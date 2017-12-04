#include <errno.h>
#include "../Include/TCPClientSocket.h"
#include "../Include/Assert.h"

CTCPClientSocket::CTCPClientSocket(CEventEngine *pEngine) : CEvent(pEngine)
{
    m_pTimer    = NULL;
    m_pCallback = NULL;
}

CTCPClientSocket::CTCPClientSocket(CEventEngine *pEngine, Int32 nFD) : CTCPSocket(nFD), CEvent(pEngine)
{
    m_pTimer    = NULL;
    m_pCallback = NULL;
}

CTCPClientSocket::~CTCPClientSocket()
{
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
        delete m_pTimer;
        m_pTimer = NULL;
    }
}

Bool CTCPClientSocket::Connect(const CSocketAddr &addr, ITCPClientSocketEvent *pCallback, Int32 nTimeout)
{
    Assert(m_nFD != -1);
    Assert(m_pCallback == NULL);

    m_pCallback = pCallback;
    if (!CTCPSocket::Connect(addr))
    {
        if (errno != EINPROGRESS)
        {
            return False;
        }
    }

    // 注册写事件
    RegisterWrite();

    // 若设置了连接超时，则创建定时器
    if (nTimeout == 0)
    {
        nTimeout = 1;
    }

    if (nTimeout >= 0)
    {
        m_pTimer = new CTimer(m_lpEngine, this);
        m_pTimer->SetTimer(nTimeout);
    }

    return True;
}

Int32 CTCPClientSocket::GetFD() const
{
    return m_nFD;
}

void CTCPClientSocket::OnWrite()
{
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
    }
    Unregister();
    m_pCallback->OnConnected(this);
}

void CTCPClientSocket::OnError(Int32 nErrCode)
{
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
    }
    Unregister();
    m_pCallback->OnConnectFailed(this, nErrCode);
}

void CTCPClientSocket::OnTimer(CTimer *pTimer)
{
    if (m_pTimer != NULL)
    {
        m_pTimer->KillTimer();
    }
    Unregister();
    m_pCallback->OnConnectTimeout(this);
}
