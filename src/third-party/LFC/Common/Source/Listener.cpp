#include "../Include/Listener.h"
#include "../Include/Assert.h"
#include "../Include/SocketUtil.h"
#include <errno.h>
#include <unistd.h>

/************************************************************************/
/* CInetListener                                                        */
/************************************************************************/

CInetListener::CInetListener(CEventEngine *pEngine, IListenerEvent *pEvent) : CEvent(pEngine)
{
	Assert(pEvent != NULL);
	m_pEvent = pEvent;
}

CInetListener::~CInetListener()
{
	if (m_listenSocket.GetFD() != -1)
	{
		Unregister();
		m_listenSocket.Close();
	}
}

Bool CInetListener::Start(const CSocketAddr &bindAddr)
{
	Bool bSuccess = False;

	do
	{
		if (!m_listenSocket.Create())
		{
			break;
		}

		if (!m_listenSocket.SetNonblock())
		{
			break;
		}

		if (!m_listenSocket.ReuseAddress())
		{
			break;
		}

		if (!m_listenSocket.Bind(bindAddr))
		{
			break;
		}

		if (!m_listenSocket.Listen())
		{
			break;
		}

		if (!RegisterRead())
		{
			break;
		}

		bSuccess = True;
	} while (0);

	return bSuccess;
}

void CInetListener::Stop()
{
	if (m_listenSocket.GetFD() != -1)
	{
		Unregister();
		m_listenSocket.Close();
	}
}

Int32 CInetListener::GetFD() const
{
	return m_listenSocket.GetFD();
}

void CInetListener::OnRead()
{
	do
	{
		Assert(m_listenSocket.GetFD() != -1);

		// 底层的事件引擎使用 EPOLL 的水平触发方式，不需要循环 accept 直到 EAGAIN 为止
		CSocketAddr address;
		Int32 nFD = m_listenSocket.Accept(address);

		if (-1 == nFD)
		{
			break;
		}
        
        // 为套接字设置 FD_CLOEXEC 选项
        SocketUtil::SetCloseOnExec(nFD);

		if (!SocketUtil::SetNonblock(nFD, True))
		{
			close(nFD);
			break;
		}
        
		Assert(m_pEvent != NULL);
		if (!m_pEvent->OnAccept(m_listenSocket.GetFD(), nFD, address))
		{
			close(nFD);
			break;
		}
	} while (0);
}

Bool CInetListener::Suspend()
{
	return Unregister();
}

Bool CInetListener::Resume()
{
	return RegisterRead();
}


/************************************************************************/
/* CUnixListener                                                        */
/************************************************************************/

CUnixListener::CUnixListener(CEventEngine *pEngine, IListenerEvent *pEvent) : CEvent(pEngine)
{
	Assert(pEvent != NULL);
	m_pEvent = pEvent;
}

CUnixListener::~CUnixListener()
{
	if (m_listenSocket.GetFD() != -1)
	{
		Unregister();
		m_listenSocket.Close();
	}
}

Bool CUnixListener::Start(LPCSTR lpszUnixPath)
{
	Bool bSuccess = False;

	do
	{
		if (!m_listenSocket.Create())
		{
			break;
		}

		if (!m_listenSocket.SetNonblock())
		{
			break;
		}

		if (!m_listenSocket.Bind(lpszUnixPath))
		{
			break;
		}

		if (!m_listenSocket.Listen())
		{
			break;
		}

		if (!RegisterRead())
		{
			break;
		}

		bSuccess = True;
	} while (0);

	return bSuccess;
}

void CUnixListener::Stop()
{
	if (m_listenSocket.GetFD() != -1)
	{
		Unregister();
		m_listenSocket.Close();
	}
}

Int32 CUnixListener::GetFD() const
{
	return m_listenSocket.GetFD();
}

void CUnixListener::OnRead()
{
	do
	{
		Assert(m_listenSocket.GetFD() != -1);

		// 底层的事件引擎使用 EPOLL 的水平触发方式，不需要循环 accept 直到 EAGAIN 为止
		Int32 nFD = m_listenSocket.Accept();

		if (-1 == nFD)
		{
			break;
		}
        
        // 为套接字设置 FD_CLOEXEC 选项
        SocketUtil::SetCloseOnExec(nFD);

		if (!SocketUtil::SetNonblock(nFD, True))
		{
			close(nFD);
			break;
		}

		// Unix 域套接字的地址为路径，此处返回一个缺省无效值
		CSocketAddr address;

		Assert(m_pEvent != NULL);
		if (!m_pEvent->OnAccept(m_listenSocket.GetFD(), nFD, address))
		{
			close(nFD);
			break;
		}
	} while (0);
}
