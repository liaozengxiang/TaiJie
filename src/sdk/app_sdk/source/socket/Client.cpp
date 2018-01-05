#include <assert.h>
#include <errno.h>
#include "Client.h"

enum EClientMsg
{
    UM_SEND_BUFFER = 1,
    UM_CLOSE
};

CClient::CClient()
    : CEvent(NULL)
{
    m_pCallback = NULL;
    m_sock = NULL;
    m_nFD = -1;

    m_thread.Stop();
}

CClient::~CClient()
{
    assert(m_sock == NULL);
    assert(m_nFD == -1);

    // 删除未发送完成的数据
    for (list<CBuffer*>::const_iterator it = m_lstBuffers.begin(); it != m_lstBuffers.end(); ++it)
    {
        delete *it;
    }
    m_lstBuffers.clear();
}

Bool CClient::Start(const CSocketAddr &server, IClientCallback *pCallback)
{
    Bool bSuccess = False;

    do
    {
        m_pCallback = pCallback;

        // 初始化消息解析器
        m_parser.Initialize(this);

        if (!m_thread.Initialize())
        {
            break;
        }

        // 设置基类的引擎指针
        m_lpEngine = &m_thread;

        m_sock = new CTCPClientSocket(&m_thread);
        if (!m_sock->Create())
        {
            break;
        }

        if (!m_sock->SetNonblock())
        {
            break;
        }

        if (!m_sock->Connect(server, this, -1))
        {
            break;
        }

        bSuccess = True;
    } while (0);

    return bSuccess;
}

void CClient::Send(const char *lpszBuf, Int32 nLen)
{
    CBuffer *pBuffer = new CBuffer();
    pBuffer->Initialize(nLen);
    pBuffer->Write(lpszBuf, nLen);

    m_thread.PostMessage(UM_SEND_BUFFER, pBuffer, NULL, this);
}

void CClient::Stop()
{
    CSemaphore sem;
    m_thread.PostMessage(UM_CLOSE, &sem, NULL, this);
    sem.WaitForSignal();
}

void CClient::OnConnected(CTCPClientSocket *pSocket)
{
    assert(pSocket == m_sock);

    m_nFD = m_sock->GetFD();
    m_sock->Detach();
    delete m_sock;
    m_sock = NULL;

    // 连接服务器成功，若在连接成功前已有数据需要发送则注册写事件，否则注册读事件
    if (m_lstBuffers.empty())
    {
        RegisterRead();
    }
    else
    {
        RegisterWrite();
    }
}

void CClient::OnConnectFailed(CTCPClientSocket *pSocket, Int32 nError)
{
    assert(pSocket == m_sock);
    delete m_sock;
    m_sock = NULL;
}

void CClient::OnConnectTimeout(CTCPClientSocket *pSocket)
{
    assert(pSocket == m_sock);
    delete m_sock;
    m_sock = NULL;
}

Int32 CClient::GetFD() const
{
    return m_nFD;
}

void CClient::OnRead()
{
    Bool bError = False;
    Int32 nError = 0;

    while (1)
    {
        char buf[1024];
        Int32 nReadSize = recv(m_nFD, buf, 1024, 0);
        if (nReadSize == -1)
        {
            nError = errno;
            if (nError == EAGAIN || nError == EINTR)
            {
                break;
            }

            bError = True;
            break;
        }
        else if (nReadSize == 0)
        {
            bError = True;
            break;
        }
        else
        {
            if (!m_parser.Parse(buf, nReadSize))
            {
                bError = True;
                nError = 8000;
                break;
            }
        }
    }

    if (bError)
    {
        OnError(nError);
    }
}

void CClient::OnWrite()
{
    Bool bError = False;
    Int32 nError = 0;

    while (!m_lstBuffers.empty())
    {
        CBuffer *pBuffer = m_lstBuffers.front();
        Int32 nSentSize = send(m_nFD, pBuffer->GetReadPtr(), pBuffer->GetUnreadLen(), 0);
        if (nSentSize == -1)
        {
            nError = errno;
            if (nError == EAGAIN || nError == EINTR)
            {
                break;
            }

            bError = True;
            break;
        }
        else if (nSentSize == 0)
        {
            bError = True;
            break;
        }
        else
        {
            pBuffer->Read(nSentSize);
            if (pBuffer->GetUnreadLen() == 0)
            {
                delete pBuffer;
                m_lstBuffers.pop_front();

                if (m_lstBuffers.empty())
                {
                    RegisterRead();
                    break;
                }
            }
        }
    }

    if (bError)
    {
        OnError(nError);
    }
}

void CClient::OnError(Int32 nErrCode)
{
    Unregister();

    assert(m_nFD != -1);
    close(m_nFD);
    m_nFD = -1;
    
    m_pCallback->OnClientDisconnected();
}

void CClient::OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam)
{
    if (nMessageID == UM_SEND_BUFFER)
    {
        // 如果此时连接未建立，则将消息暂存到队列中，待连接成功后再注册写事件
        if (m_lstBuffers.empty() && m_nFD != -1)
        {
            RegisterWrite();
        }

        CBuffer *pBuffer = (CBuffer *)wParam;
        m_lstBuffers.push_back(pBuffer);
    }
    else if (nMessageID == UM_CLOSE)
    {
        // TODO: 隐患, 若 m_sock 不为空，则表示连接异步操作未完成，会挂掉
        // 但在此处无法通过 m_sock 取消读写事件，CTCPClientSocket 设计不够友好
        if (m_nFD != -1)
        {
            Unregister();
            close(m_nFD);
            m_nFD = -1;
        }

        CSemaphore *sem = (CSemaphore *)wParam;
        sem->PostSignal();
    }
}

Bool CClient::OnParsePacket(const MsgHeader *pMsgHdr, const char *lpszContent, Int32 nLen)
{
    return m_pCallback->OnParsePacket(pMsgHdr, lpszContent, nLen);
}
