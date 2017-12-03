#ifndef ___EVENT__ENGINE__20140422___
#define ___EVENT__ENGINE__20140422___

#include <vector>
#include "OSType.h"
#include "MessageEvent.h"

class CEvent;
class CEventEngine
{
public:
    CEventEngine();
    virtual ~CEventEngine();

public:
    Bool Initialize();

    Bool AddEvent(CEvent *lpEvent, Int32 nEventType);
    Bool ModEvent(CEvent *lpEvent, Int32 nEventType);
    Bool DelEvent(CEvent *lpEvent);

    Bool PostMessage(Int32 nMessageID, void *wParam, void *lParam, IMessageEvent *pMsgEvent);

    void* SetPrivatePtr(void *ptr);
    void* GetPrivatePtr() const;

public:
    virtual void Loop(Int32 nTimeout);

protected:
    void EventTest(Int32 nTimeout);

protected:
    Int32								m_nEpollFD;								// epoll 句柄
    std::vector<CEvent *>				m_vecDeleting;							// 正在回调时上层需要删除的元素
    volatile Bool						m_bOnEvent;								// 标志是否在处理 I/O 事件
    void*								m_pPrivatePtr;							// 用户私有数据
};

#endif
