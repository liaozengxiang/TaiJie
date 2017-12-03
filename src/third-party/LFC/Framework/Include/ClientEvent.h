/*
 * class CClientEvent
 * 客户端套接字事件对象，派生类必须从该类继承，实现消息解析和执行消息的函数
 * Copyright (C) 2016. All rights reserved. Powered by Lory Liao
 */

#ifndef CLIENT__EVENT__20160401
#define CLIENT__EVENT__20160401

#include <Common/Common.h>

class CClientEvent;
struct IMessageDispatcher
{
	virtual void DispatchMessage(CClientEvent *pClientEvent, void *pMsg) = 0;
};

enum EClientEventMsgID
{
	eClientMsgSyncBuffer = 1,
	eClientMsgDetach
};

class CClientEvent : public CEvent, public IMessageEvent, public ITimerEvent, public CReference
{
protected:
	CClientEvent(Int32 nFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher);
	virtual ~CClientEvent();									// 该类的对象必须通过 CReference::Release 方法来析构

public:
	virtual Bool Initialize();									// 派生类若重载该虚函数，必须显式调用基类的此函数完成基类的初始化
	virtual void HandleMessage(void *pMsg, Bool bAsync) = 0;	// 在派生类中，可以创建用户处理器对象，并让其处理

protected:
	// CEvent
	virtual Int32 GetFD() const;
	virtual void  OnRead();
	virtual void  OnWrite();
	virtual void  OnError(Int32 nErrCode);

	// IMessageEvent
	virtual void OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam);

	// ITimerEvent
	virtual void OnTimer(CTimer *pTimer);

protected:
	virtual Bool Parse(LPCSTR lpszBuffer, Int32 nLen) = 0;		// 在派生类中，可以通过消息解析器流式解析，产生完整消息后通过 DispatchMessage 派遣消息
	virtual void OnMessageSent() = 0;							// 异步消息发送完毕后调用，上层可根据情况重新注册读事件

protected:
	void  DispatchMessage(void *pMsg);
	Int32 Send(LPCSTR lpszBuff, Int32 nLen, Bool bAsync);
	Int32 Send(const std::string &strMsg, Bool bAsync);
	Int32 Send(CBuffer *pBuffer, Bool bAsync);
	void  Detach(Bool bAsync);									// 将套接字从该对象上解绑，取消其上的 I/O 事件，并删除对象
	void  Close();

protected:
	Int32						m_nFD;							// 已连接的套接字
	CSocketAddr					m_address;						// 对端地址
	IMessageDispatcher*			m_pDispatcher;					// 消息派遣对象，派生类收到一个完整消息后通过该对象将消息派遣出去
	std::list<CBuffer*>			m_lstSendBuffers;
	CTimer*						m_pTimer;
};

#endif
