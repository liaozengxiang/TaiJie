#include "../Include/RtspClientEvent.h"

CRtspClientEvent::CRtspClientEvent(Int32 nFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher, IRtspHandlerFactory *pHandlerFactory, Int64 lLinkTimeout)
	: CClientEvent(nFD, addr, pEngine, pDispatcher), m_parser(this)
{
	Assert(pHandlerFactory != NULL);

	m_pHandlerFactory = pHandlerFactory;
	m_bKeepAlive = True;
	m_lLinkTimeout = lLinkTimeout;
	m_lLinkActiveTime = OS::Milliseconds();
}

CRtspClientEvent::~CRtspClientEvent()
{
}

Bool CRtspClientEvent::Parse(LPCSTR lpszBuffer, Int32 nLen)
{
	m_lLinkActiveTime = OS::Milliseconds();
	return m_parser.Parse(lpszBuffer, nLen);
}

void CRtspClientEvent::HandleMessage(void *pMsg, Bool bAsync)
{
	// 此函数被调用时，有二种可能
	// (1) 上层启动服务器未指定工作线程池，则该函数由对应的 I/O 线程同步调用
	// (2) 上层启动服务器指定了工作线程池，则该函数由工作线程池调用，且调用之前已对 CHttpClientEvent 增加引用计数

	// 取出 Http 请求消息
	CRtspReqMessage *pReqMsg = static_cast<CRtspReqMessage *>(pMsg);

	Assert(pReqMsg != NULL);
	CAutoPtr<CRtspReqMessage> ptrDel(pReqMsg);

	// 创建消息处理器对象
	IRtspHandler *pHandler = m_pHandlerFactory->Create(pReqMsg);
	Assert(pHandler != NULL);

	do
	{
		if (!pHandler->OnPreCheckRequest(m_address, m_nFD, pReqMsg))
		{
			Detach(bAsync);
			break;
		}

		// 检查请求消息是否正确
		ERtspCode eCode = pHandler->OnCheckRequest(pReqMsg);

		// 创建 Rtsp 响应对象
		CRtspResMessage resMsg;
		resMsg.SetCode(eCode, CSingleton<CRtspCode>::GetInstancePtr()->GetRtspCodeDesc(eCode), "RTSP/1.0");

		if (eCode == eRtspOK)
		{
			// 只有在检查消息返回成功时才执行请求
			pHandler->OnHandleRequest(pReqMsg, &resMsg);
		}

		pHandler->OnPreResponse(&resMsg);
		Send(resMsg.GetPacket(), bAsync);

	} while (0);

	// 删除创建的处理器对象
	pHandler->Release();
}

void CRtspClientEvent::OnMessageSent()
{
	if (m_bKeepAlive)
	{
		RegisterRead();
	}
	else
	{
		Unregister();
		Close();
	}
}

void CRtspClientEvent::OnTimer(CTimer *pTimer)
{
	if (OS::Milliseconds() - m_lLinkActiveTime >= m_lLinkTimeout)
	{
		Unregister();
		Close();
	}

	CClientEvent::OnTimer(pTimer);
}

void CRtspClientEvent::OnParseRtspPacket(const std::string &strHeader, const std::string &strBody)
{
	// 该函数由套接字 I/O 线程调用
	// 由于消息处理器可能是线程池，有可能在之前的消息请求过程中关闭了连接，此时该请求可以忽略
	do
	{
		if (-1 == m_nFD)
		{
			break;
		}

		// 创建请求消息
		CRtspReqMessage *pMsg = new CRtspReqMessage();
		if (!pMsg->SetMessage(strHeader, strBody))
		{
			// 请求的消息无法正常解析，直接关闭连接
			delete pMsg;
			Unregister();
			Close();
			break;
		}

		// 派遣消息
		DispatchMessage(pMsg);
	} while (0);
}
