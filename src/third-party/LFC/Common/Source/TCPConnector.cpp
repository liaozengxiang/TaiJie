#include <errno.h>
#include "../Include/TCPConnector.h"

CTCPConnector::CTCPConnector(CEventEngine *pEngine)
    : CEvent(pEngine)
{
    m_pTimer = NULL;
}

CTCPConnector::~CTCPConnector()
{
    Unregister();
        
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
        delete m_pTimer;
        m_pTimer = NULL;
    }
}

Bool CTCPConnector::Connect(const CSocketAddr &addr, CWeakPtr<ITCPConnectorCallback> wpCallback, Int32 nTimeout)
{
    Bool bSuccess = False;

    do
    {
        m_wpCallback = wpCallback;
        if (!m_socket.Create())
        {
            break;
        }

        if (!m_socket.SetNonblock())
        {
            break;
        }

        if (!m_socket.Connect(addr))
        {
            if (errno != EINPROGRESS)
            {
                break;
            }
        }

        // 注册可写事件
        if (!RegisterWrite())
        {
            break;
        }
        // 若设置了连接超时，则创建定时器
        if (nTimeout == 0)
        {
            nTimeout = 1;
        }
        if (nTimeout > 0)
        {
            m_pTimer = new CTimer(m_lpEngine, this);
            m_pTimer->SetTimer(nTimeout);
        }

        bSuccess = True;
    } while (0);

    return bSuccess;
}

void CTCPConnector::Detach()
{
    m_socket.Detach();
}

void CTCPConnector::OnWrite()
{
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
    }

    Unregister();

    CSharedPtr<ITCPConnectorCallback> spCallback = m_wpCallback.Lock();
    if (spCallback != NULL)
    {
        spCallback->OnConnected();
    }
}

void CTCPConnector::OnError(Int32 nErrCode)
{
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
    }

    Unregister();

    CSharedPtr<ITCPConnectorCallback> spCallback = m_wpCallback.Lock();
    if (spCallback != NULL)
    {
        spCallback->OnConnectFailed(nErrCode);
    }
}

void CTCPConnector::OnTimer(CTimer *pTimer)
{
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
    }

    Unregister();

    CSharedPtr<ITCPConnectorCallback> spCallback = m_wpCallback.Lock();
    if (spCallback != NULL)
    {
        spCallback->OnConnectTimeout();
    }
}
