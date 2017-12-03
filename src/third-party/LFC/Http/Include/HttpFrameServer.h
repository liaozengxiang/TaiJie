#ifndef HTTP__FRAME__SERVER__20160401
#define HTTP__FRAME__SERVER__20160401

#include <Common/Common.h>
#include <Framework/FrameServer.h>
#include "HttpClientEvent.h"

class CHttpFrameServer : public CFrameServer
{
public:
	CHttpFrameServer(CEventEngine *pListenEngine);
	virtual ~CHttpFrameServer();

public:
	/*
	 * 功能: 此函数用于启动框架服务
	 * 参数: 
	 *       @address    -- Inet 监听地址
	 *       @vecAsyncIo -- 异步 I/O 驱动线程
	 *       @pWookPool  -- 消息处理线程池，指示为 NULL 时表示消息的处理由套接字的 I/O 事件线程执行
	 */
	Bool Start(const CSocketAddr &address, const std::vector<CEventEngine *> &vecAsyncIo, CThreadPool *pWorkPool, IHttpHandlerFactory *pHandlerFactory);
	void SetLinkTimeout(Int64 lLinkTimeout);					// 设置以毫秒为单位的 Http 长连接超时时间

protected:
	/*
	 * 功能: 创建具体的客户端套接字事件类，该类对象必须是 CClientEvent 的派生类，用于解析和派遣消息
	 * 参数: 
	 * 返回: 返回具体的客户端套接字事件类对象
	 */
	virtual CClientEvent* CreateClientEvent(Int32 nClientFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher);

protected:
	IHttpHandlerFactory*					m_pHandlerFactory;
	Int64									m_lLinkTimeout;
};

#endif
