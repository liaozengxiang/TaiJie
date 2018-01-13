#ifndef ___TIMER__H__20140506___
#define ___TIMER__H__20140506___

#include "OSType.h"
#include "Event.h"

class CTimer;
struct ITimerEvent
{
	virtual ~ITimerEvent() {}
	virtual void OnTimer(CTimer *pTimer) = 0;
};

class CEventEngine;
class CTimer : public CEvent
{
public:
	CTimer(CEventEngine *lpEngine, ITimerEvent *lpTimerEvent = NULL);		// 构造定时器对象，并指定定时事件的接收者
	virtual ~CTimer();

	Bool SetTimer(Int32 nInterval);										// 设置定时器
	void KillTimer();													// 取消定时器

protected:
	virtual Int32 GetFD() const;
	virtual void  OnRead();
	virtual void  OnWrite();
	virtual void  OnError(Int32 nErrCode);

protected:
	Int32			m_nTimerFD;											// 定时器文件句柄
	ITimerEvent*	m_lpTimerEvent;
};

#endif
