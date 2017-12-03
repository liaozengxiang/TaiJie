#ifndef ___LISTENER__20150509___
#define ___LISTENER__20150509___

#include "OSType.h"
#include "InetListenSocket.h"
#include "UnixListenSocket.h"
#include "EventEngine.h"
#include "Event.h"

struct IListenerEvent
{
	virtual ~IListenerEvent() {}
	virtual Bool OnAccept(Int32 nListenFD, Int32 nClientFD, const CSocketAddr &addr) = 0;
};

class CInetListener : public CEvent
{
public:
	CInetListener(CEventEngine *pEngine, IListenerEvent *pEvent);
	virtual ~CInetListener();

public:
	Bool Start(const CSocketAddr &bindAddr);
	Bool Suspend();
	Bool Resume();
	void Stop();
	virtual Int32 GetFD() const;

protected:
	virtual void OnRead();

protected:
	IListenerEvent*			m_pEvent;				// 监听事件回调接口
	CInetListenSocket		m_listenSocket;			// 服务端监听套接字
};


class CUnixListener : public CEvent
{
public:
	CUnixListener(CEventEngine *pEngine, IListenerEvent *pEvent);
	virtual ~CUnixListener();

public:
	Bool Start(LPCSTR lpszUnixPath);
	void Stop();
	virtual Int32 GetFD() const;

protected:
	virtual void OnRead();

protected:
	IListenerEvent*			m_pEvent;				// 监听事件回调接口
	CUnixListenSocket		m_listenSocket;			// 服务端监听套接字
};

#endif
