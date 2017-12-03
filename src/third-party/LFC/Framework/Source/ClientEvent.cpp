#include "../Include/ClientEvent.h"
#include <errno.h>
#include <unistd.h>

CClientEvent::CClientEvent(Int32 nFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher)
	: CEvent(pEngine), m_address(addr)
{
	m_nFD = nFD;

	Assert(pDispatcher != NULL);
	m_pDispatcher = pDispatcher;

	// 创建定时器，用于检测链接超时
	m_pTimer = new CTimer(pEngine, this);
	m_pTimer->SetTimer(1000);
}

CClientEvent::~CClientEvent()
{
	// 删除定时器
	m_pTimer->KillTimer();
	delete m_pTimer;
	m_pTimer = NULL;

	// 析构函数中不能调用(或间接调用)本类派生类的虚函数，因为析构对象时派生类对象优先析构
	// Unregister();
	Close();

 	// 删除未发送完的数据
 	for (std::list<CBuffer *>::const_iterator it = m_lstSendBuffers.begin(); it != m_lstSendBuffers.end(); ++it)
 	{
 		delete *it;
 	}
 	m_lstSendBuffers.clear();
}

Bool CClientEvent::Initialize()
{
	// 监听套接字上的读事件
	return RegisterRead();
}

Int32 CClientEvent::GetFD() const
{
	return m_nFD;
}

void CClientEvent::OnRead()
{
	#define MSG_RECV_BUFF_SIZE		65536
	char szRecvBuffer[MSG_RECV_BUFF_SIZE];

	Bool bClose = False;
	Int32 nError = 0;

	while (1)
	{
		Int32 nReadSize = SocketUtil::Recv(m_nFD, szRecvBuffer, MSG_RECV_BUFF_SIZE);
		if (nReadSize == 0)
		{
			// 对端关闭连接
			bClose = True;
			break;
		}
		else if (nReadSize == -1)
		{
			if (errno == EAGAIN || errno == EINTR)
			{
				break;
			}

			// 套接字错误
			bClose = True;
			nError = errno;
			break;
		}

		if (!Parse(szRecvBuffer, nReadSize))
		{
			// 解析出错
			bClose = True;
			nError = -1;
			break;
		}

		// 如果在某一次处理中解除了套接字，不应再继续处理该套接字上任何操作，退出读循环
		if (-1 == m_nFD)
		{
			bClose = False;
			break;
		}
	}

	if (bClose)
	{
		OnError(nError);
	}
}

void CClientEvent::OnWrite()
{
	Bool bClose = False;
	Int32 nError = 0;

	while (!m_lstSendBuffers.empty())
	{
		CBuffer *pBuffer = m_lstSendBuffers.front();
		Int32 nNeedSend = pBuffer->GetWriteLen() - pBuffer->GetReadLen();
		Int32 nSentSize = SocketUtil::Send(m_nFD, pBuffer->GetReadPtr(), nNeedSend);
		if (nSentSize == 0)
		{
			// 对端关闭连接
			bClose = True;
			break;
		}
		else if (nSentSize == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;
			}

			// 套接字错误
			bClose = True;
			nError = errno;
			break;
		}

		pBuffer->Read(nSentSize);
		if (nSentSize == nNeedSend)
		{
			delete pBuffer;
			m_lstSendBuffers.pop_front();
		}
	}

	if (bClose)
	{
		OnError(nError);
	}
	else
	{
		if (m_lstSendBuffers.empty())
		{
			OnMessageSent();
		}
	}
}

void CClientEvent::OnError(Int32 nErrCode)
{
	Unregister();
	Close();
}

void CClientEvent::OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam)
{
	if (eClientMsgSyncBuffer == nMessageID)
	{
		CBuffer *pBuffer = static_cast<CBuffer *>(wParam);
		Assert(pBuffer != NULL);

		Send(pBuffer, False);

		// 异步处理完后释放引用计数
		Release();
	}
	else if (eClientMsgDetach)
	{
		Detach(False);

		// 通知调用线程该操作已异步完成
		CSemaphore *pSem = static_cast<CSemaphore *>(wParam);
		pSem->PostSignal();
	}
}

/*
 * 功能: 每秒定时检测该对象是否可删除
 * 说明: 
 *       派生类若需要实现长链接超时时删除对象，可重载该函数
 *       检测在指定的时间内若无请求，可主动关闭套接字，并将 m_nFD 置为 -1
 *       随后调用基类的 CClientEvent::OnTimer
 * 示例: 
 *       void CSpecifiedClientEvent::OnTimer(CTimer *pTimer)
 *       {
 *           // 检测链接是否超时的相关代码，略
 *           if (bLinkTimeout)
 *           {
 *               Unregister();
 *               Close();
 *           }
 *
 *           CClientEvent::OnTimer(pTimer);
 *       }
 */
void CClientEvent::OnTimer(CTimer *pTimer)
{
	if (-1 == m_nFD && GetRef() == 1)
	{
		Release();
	}
}

void CClientEvent::DispatchMessage(void *pMsg)
{
	m_pDispatcher->DispatchMessage(this, pMsg);
}

/*
| --------------------------------------------------------------------------------
| 向套接字对端发送消息
| --------------------------------------------------------------------------------
|
| lpszBuff: 表示要发送到对端的内容
| nLen:     表示要发送内容的长度
|
| bAsync:   指示调用该函数的线程与 I/O 事件线程是否在同一线程上
|           若调用线程与 I/O 线程处于同一线程，则传递 False, 否则传递 True
|           消息处理可以由用户的工作线程执行，在发送内容时，应当处理线程安全问题
*/
Int32 CClientEvent::Send(LPCSTR lpszBuff, Int32 nLen, Bool bAsync)
{
	Assert(lpszBuff != NULL);
	Assert(nLen > 0);

	CBuffer *pBuffer = new CBuffer();
	if (!pBuffer->Initialize(nLen))
	{
		delete pBuffer;
		return 0;
	}

	pBuffer->Write(lpszBuff, nLen);
	Send(pBuffer, bAsync);

	return nLen;
}

/*
| --------------------------------------------------------------------------------
| 向套接字对端发送消息
| --------------------------------------------------------------------------------
|
| Refer to function:
|     Int32 CClientEvent::Send(LPCSTR lpszBuff, Int32 nLen, Bool bAsync);
*/
Int32 CClientEvent::Send(const std::string &strMsg, Bool bAsync)
{
	return Send(strMsg.c_str(), strMsg.length(), bAsync);
}

/*
| --------------------------------------------------------------------------------
| 向套接字对端发送消息
| --------------------------------------------------------------------------------
|
| pBuffer 表示要发送的数据包
*/
Int32 CClientEvent::Send(CBuffer *pBuffer, Bool bAsync)
{
	Int32 nLen = pBuffer->GetWriteLen();

	if (bAsync)
	{
		// 如果是异步发送，则需要把待发送的消息同步到线程中，增加引用计数
		AddRef();
		m_lpEngine->PostMessage(eClientMsgSyncBuffer, pBuffer, NULL, this);
	}
	else
	{
		if (m_lstSendBuffers.empty())
		{
			RegisterRW();
		}

		m_lstSendBuffers.push_back(pBuffer);
	}

	return nLen;
}

void CClientEvent::Detach(Bool bAsync)
{
	if (bAsync)
	{
		// 为了让 Detach 的主调线程调用完 Detach 函数后确保套接字已从 epoll 上移除
		// 此得使用信号量等待该操作完成
		CSemaphore sem;
		m_lpEngine->PostMessage(eClientMsgDetach, &sem, NULL, this);
		sem.WaitForSignal();
	}
	else
	{
		// 将套接字从该对象上解绑，取消其上的 I/O 事件
		Unregister();
		m_nFD = -1;
	}
}

void CClientEvent::Close()
{
	// 将套接字从该对象上解绑，取消其上的 I/O 事件
	if (m_nFD != -1)
	{
		close(m_nFD);
		m_nFD = -1;
	}
}
