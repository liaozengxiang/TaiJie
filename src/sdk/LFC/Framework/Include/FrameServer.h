/*
 * class CFrameServer
 * 服务器框架类，派生类必须从该类继承，并添加一个启动函数，重载创建具体客户端 CClientEvent 对象的代码
 * Copyright (C) 2016. All rights reserved. Powered by Lory Liao
 */

#ifndef FRAME__SERVER__20160401
#define FRAME__SERVER__20160401

#include <Common/Common.h>
#include "ClientEvent.h"

class CFrameServer : public IListenerEvent, public IMessageDispatcher
{
public:
	CFrameServer(CEventEngine *pListenEngine);
	virtual ~CFrameServer();

protected:
	/*
	 * 功能: 此函数用于启动框架服务
	 * 参数: 
	 *       @address    -- Inet 监听地址
	 *       @vecAsyncIo -- 异步 I/O 驱动线程
	 *       @pWookPool  -- 消息处理线程池，指示为 NULL 时表示消息的处理由套接字的 I/O 事件线程执行
	 */
	Bool Start(const CSocketAddr &address, const std::vector<CEventEngine *> &vecAsyncIo, CThreadPool *pWorkPool);

public:
	Bool Suspend();			// 暂停监听
	Bool Resume();			// 恢复监听
	void Stop();			// 停止服务

protected:
	// IListenerEvent
	virtual Bool OnAccept(Int32 nListenFD, Int32 nClientFD, const CSocketAddr &addr);

	/*
	 * 功能: 创建具体的客户端套接字事件类，该类对象必须是 CClientEvent 的派生类，用于解析和派遣消息
	 * 参数: 
	 * 返回: 返回具体的客户端套接字事件类对象
	 */
	virtual CClientEvent* CreateClientEvent(Int32 nClientFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher) = 0;

	/*
	 * 功能: 消息派遣函数，CClientEvent 的派生类接收到了一个完整的请求 pMsg, 需要将消息派遣到工作线程中同步执行
	 * 参数: 
	 *       @pClientEvent -- 产生消息的客户端套接字事件对象
	 *       @pMsg         -- CClientEvent 派生类对象产生的消息，只有该类能识别消息的具体类型
	 * 说明: 建议在 CClientEvent 派生类中，根据消息类型动态产生客户端消息处理器，可在 CFrameServer 派生类的 Start 函数中传递抽象工厂对象来创建
	 */
	virtual void DispatchMessage(CClientEvent *pClientEvent, void *pMsg);

protected:
	CInetListener				m_listener;								// 监听器
	std::vector<CEventEngine *>	m_vecAsyncIo;							// 异步处理线程池，处理套接字上的 I/O 事件
	CThreadPool*				m_pWorkPool;							// 消息处理工作线程，若为空，则使用对应的 I/O 事件线程驱动
};

#endif
