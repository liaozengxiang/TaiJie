#include <errno.h>
#include <unistd.h>
#include "../Include/HttpClient.h"

CHttpClient::CHttpClient(CEventEngine *pEngine)
    : CEvent(pEngine), m_connector(pEngine)
{
    m_nFD = -1;
}


CHttpClient::~CHttpClient()
{
    Unregister();

    if (m_nFD != -1)
    {
        close(m_nFD);
        m_nFD = -1;
    }

    // 删除未发送的数据块
    for (list<CBuffer*>::const_iterator it = m_lstBuffers.begin(); it != m_lstBuffers.end(); ++it)
    {
        delete *it;
    }
    m_lstBuffers.clear();
}

Bool CHttpClient::Request(const CSocketAddr &addr, CWeakPtr<IHttpClientRequest> wpCallback, Int32 nTimeout)
{
    m_wpCallback = wpCallback;

    CSharedPtr<CHttpClient> spHttpClient = SharedFromThis();
    CSharedPtr<ITCPConnectorCallback> spCallback = dynamic_pointer_cast<ITCPConnectorCallback>(spHttpClient);

    return m_connector.Connect(addr, spCallback, nTimeout);
}

void CHttpClient::OnConnected()
{
    m_nFD = m_connector.GetFD();
    m_connector.Detach();

    FillSendData();

    if (!m_lstBuffers.empty())
    {
        RegisterWrite();
    }
}

void CHttpClient::OnConnectFailed(Int32 nError)
{
    CSharedPtr<IHttpClientRequest> spCallback = m_wpCallback.Lock();
    if (spCallback != NULL)
    {
        spCallback->OnHttpClientClosed(nError);
    }
}

void CHttpClient::OnConnectTimeout()
{
    OnError(ETIMEDOUT);
}

void CHttpClient::OnRead()
{
    char buff[8192];
    Bool bClose = False;
    Int32  nError;
    while (1)
    {
        Int32 nRecvSize = recv(m_nFD, buff, 8192, 0);
        if (nRecvSize < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
            {
                break;
            }

            nError = errno;
            bClose = True;
            break;
        }
        else if (nRecvSize == 0)
        {
            nError = 0;
            bClose = True;
            break;
        }
        
        // 成功接收到 nRecvSize 字节的数据，回调给上层
        CSharedPtr<IHttpClientRequest> spCallback = m_wpCallback.Lock();
        if (spCallback != NULL)
        {
            spCallback->OnHttpClientRecv(buff, nRecvSize);
        }
    }

    if (bClose)
    {
        OnError(nError);
    }
}

void CHttpClient::OnWrite()
{
    Bool bClose = False;

    while (!m_lstBuffers.empty())
    {
        CBuffer *pBuffer = m_lstBuffers.front();
        Int32 nSentSize = send(m_nFD, pBuffer->GetReadPtr(), pBuffer->GetUnreadLen(), 0);
        if (nSentSize < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
            {
                break;
            }
            bClose = True;
            break;
        }
        else if (nSentSize == 0)
        {
            bClose = True;
            break;
        }

        // 成功发送 nSentSize 字节的数据
        pBuffer->Read(nSentSize);
        if (pBuffer->GetUnreadLen() == 0)
        {
            delete pBuffer;
            m_lstBuffers.pop_front();

            if (m_lstBuffers.empty())
            {
                // 队列为空，尝试继续获取待发送的数据
                FillSendData();

                // 发送的数据已全部完成，注册读事件，接收服务器端响应
                if (m_lstBuffers.empty())
                {
                    RegisterRead();
                }
            }
        }
    }
    if (bClose)
    {
        OnError(errno);
    }
}

void CHttpClient::OnError(Int32 nErrCode)
{
    Unregister();

    CSharedPtr<IHttpClientRequest> spCallback = m_wpCallback.Lock();
    if (spCallback != NULL)
    {
        spCallback->OnHttpClientClosed(nErrCode);
    }
}

void CHttpClient::FillSendData()
{
    CSharedPtr<IHttpClientRequest> spCallback = m_wpCallback.Lock();
    if (spCallback != NULL)
    {
        string str;
        if (spCallback->OnHttpClientSend(str))
        {
            Assert(!str.empty());

            CBuffer *pBuffer = new CBuffer();
            pBuffer->Initialize(str.length());
            pBuffer->Write(str.c_str(), str.length());

            m_lstBuffers.push_back(pBuffer);
        }
    }
}
