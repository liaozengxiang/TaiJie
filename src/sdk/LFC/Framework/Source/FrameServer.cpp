#include "../Include/FrameServer.h"


/************************************************************************/
/* CSyncHandleTask 同步执行任务类                                       */
/************************************************************************/

class CSyncHandleTask : public IWorkTask
{
public:
	CSyncHandleTask(CClientEvent *pClientEvent, void *pMsg)
	{
		m_pClientEvent = pClientEvent;
		m_pMsg         = pMsg;
	}

	virtual void OnWork()
	{
		m_pClientEvent->HandleMessage(m_pMsg, True);
		delete this;
	}

private:
	CClientEvent* m_pClientEvent;
	void*         m_pMsg;
};


/************************************************************************/
/* CFrameServer                                                         */
/************************************************************************/

CFrameServer::CFrameServer(CEventEngine *pListenEngine) : m_listener(pListenEngine, this)
{
	m_pWorkPool = NULL;
}

CFrameServer::~CFrameServer()
{
	Stop();
}

Bool CFrameServer::Start(const CSocketAddr &address, const std::vector<CEventEngine *> &vecAsyncIo, CThreadPool *pWorkPool)
{
	Assert(!vecAsyncIo.empty());

	m_vecAsyncIo = vecAsyncIo;
	m_pWorkPool  = pWorkPool;
	return m_listener.Start(address);
}

Bool CFrameServer::Suspend()
{
	return m_listener.Suspend();
}

Bool CFrameServer::Resume()
{
	return m_listener.Resume();
}

void CFrameServer::Stop()
{
	m_listener.Stop();
}

Bool CFrameServer::OnAccept(Int32 nListenFD, Int32 nClientFD, const CSocketAddr &addr)
{
	// 根据客户端套接字对异步线程数取余，取对应的线程处理该套接字上的 I/O 事件，可以保证每个线程上的负载均衡
	CEventEngine *pEngine = m_vecAsyncIo[nClientFD % m_vecAsyncIo.size()];
	Assert(pEngine != NULL);

	CClientEvent *pClientEvent = CreateClientEvent(nClientFD, addr, pEngine, this);
	Assert(pClientEvent != NULL);

	if (!pClientEvent->Initialize())
	{
		pClientEvent->Release();
		return False;
	}

	return True;
}

void CFrameServer::DispatchMessage(CClientEvent *pClientEvent, void *pMsg)
{
	if (m_pWorkPool != NULL)
	{
		// 如果指定了消息处理线程，则将消息同步到工作线程上执行
		CSyncHandleTask *pHandleTask = new CSyncHandleTask(pClientEvent, pMsg);
		if (!m_pWorkPool->AddTask(pHandleTask))
		{
			delete pHandleTask;
		}
	}
	else
	{
		// 如果未指定消息处理线程，则消息直接由 I/O 事件线程执行
		pClientEvent->HandleMessage(pMsg, False);
	}
}
