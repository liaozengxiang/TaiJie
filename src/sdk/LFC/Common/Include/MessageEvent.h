#ifndef ___MESSAGE__EVENT__20150503___
#define ___MESSAGE__EVENT__20150503___

#include "Event.h"

struct IMessageEvent
{
	virtual ~IMessageEvent() {}
	virtual void OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam) = 0;
};

class CMessageEvent : public CEvent
{
public:
	CMessageEvent(CEventEngine *pEngine);
	virtual ~CMessageEvent();

public:
	Bool Create(Int32 nMessageID, void *wParam, void *lParam, IMessageEvent *pMsgEvent);

protected:
	virtual Int32 GetFD() const;
	virtual void  OnWrite();
	virtual void  OnRead();
	virtual void  OnError(Int32 nErrCode);

protected:
	Int32			m_nMessageID;
	void*			m_wParam;
	void*			m_lParam;
	IMessageEvent*	m_pMsgEvent;
	Int32			m_nEventFD;
};

#endif
