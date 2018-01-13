#define __USE_GNU

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <algorithm>
#include <pthread.h>
#include "../Include/Event.h"
#include "../Include/EventEngine.h"
#include "../Include/Macro.h"

// 定义 epoll_wait 接收事件的队列长度
static const Int32 s_nWaitEvents = 128;

CEventEngine::CEventEngine()
{
    m_vecDeleting.reserve(s_nWaitEvents);

    m_nEpollFD    = -1;
    m_bOnEvent    = False;
    m_pPrivatePtr = NULL;
}

CEventEngine::~CEventEngine()
{
    if (m_nEpollFD != -1)
    {
        close(m_nEpollFD);
        m_nEpollFD = -1;
    }
}

Bool CEventEngine::Initialize()
{
    // Since Linux 2.6.8, the size argument is unused, but must be greater than zero.
    m_nEpollFD = epoll_create(32000);
    return (m_nEpollFD != -1);
}

Bool CEventEngine::AddEvent(CEvent *lpEvent, Int32 nEventType)
{
    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    struct epoll_event ev;
    ev.data.ptr = (void *)lpEvent;
    ev.events   = 0;
    if (nEventType & EVENT_TYPE::EVENT_READ)
    {
        ev.events |= EPOLLIN;
    }

    if (nEventType & EVENT_TYPE::EVENT_WRITE)
    {
        ev.events |= EPOLLOUT;
    }

    if (-1 == epoll_ctl(m_nEpollFD, EPOLL_CTL_ADD, lpEvent->GetFD(), &ev))
    {
        return False;
    }

    return True;
}

Bool CEventEngine::ModEvent(CEvent *lpEvent, Int32 nEventType)
{
    struct epoll_event ev;
    ev.data.ptr = (void *)lpEvent;
    ev.events   = 0;
    if (nEventType & EVENT_TYPE::EVENT_READ)
    {
        ev.events |= EPOLLIN;
    }

    if (nEventType & EVENT_TYPE::EVENT_WRITE)
    {
        ev.events |= EPOLLOUT;
    }

    if (-1 == epoll_ctl(m_nEpollFD, EPOLL_CTL_MOD, lpEvent->GetFD(), &ev))
    {
        return False;
    }

    return True;
}

Bool CEventEngine::DelEvent(CEvent *lpEvent)
{
    struct epoll_event ev;
    ev.events = 0;

    // ev is ignored, but this should not be NULL. For compatible with linux 2.6.9 before
    if (-1 == epoll_ctl(m_nEpollFD, EPOLL_CTL_DEL, lpEvent->GetFD(), &ev))
    {
        return False;
    }

    if (m_bOnEvent)
    {
        m_vecDeleting.push_back(lpEvent);
    }

    return True;
}

void CEventEngine::EventTest(Int32 nTimeout)
{
    struct epoll_event events[s_nWaitEvents];
    CEvent *lpEvent;
    Int32 i, nEvents;

    nEvents = epoll_wait(m_nEpollFD, events, s_nWaitEvents, nTimeout);
    if (-1 == nEvents)
    {
        if (errno != EINTR)
        {
            // perror("Failed to wait for an epoll event." << strerror(errno)");
        }
        return;
    }
    else if (0 == nEvents)
    {
        return;
    }

    // 标志正在处理 I/O 事件，此期间所有被删除的事件都将存放在 m_vecDeleting 中
    m_bOnEvent = True;

    for (i = 0; i < nEvents; i ++)
    {
        lpEvent = static_cast<CEvent *>(events[i].data.ptr);

        // 如果此事件已被删除，则忽略之
        if (std::find(m_vecDeleting.begin(), m_vecDeleting.end(), lpEvent) != m_vecDeleting.end())
        {
            continue;
        }

        if ((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR))
        {
            Int32 nError;
            socklen_t nLen = sizeof(Int32);
            if (getsockopt(lpEvent->GetFD(), SOL_SOCKET, SO_ERROR, (char *)&nError, &nLen) == -1)
            {
                nError = errno;
            }

            lpEvent->OnError(nError);
            continue;
        }

        if (events[i].events & EPOLLIN)
        {
            lpEvent->OnRead();
        }

        if (events[i].events & EPOLLOUT)
        {
            // 如果此事件在 OnRead 中已被删除，则忽略写事件
            if (std::find(m_vecDeleting.begin(), m_vecDeleting.end(), lpEvent) != m_vecDeleting.end())
            {
                continue;
            }

            lpEvent->OnWrite();
        }
    }

    m_bOnEvent = False;
    m_vecDeleting.clear();
}

void CEventEngine::Loop(Int32 nTimeout)
{
    EventTest(nTimeout);
}

Bool CEventEngine::PostMessage(Int32 nMessageID, void *wParam, void *lParam, IMessageEvent *pMsgEvent)
{
    CMessageEvent *pMsg = new CMessageEvent(this);
    return pMsg->Create(nMessageID, wParam, lParam, pMsgEvent);
}

void* CEventEngine::SetPrivatePtr(void *ptr)
{
    void *pOldPtr = m_pPrivatePtr;
    m_pPrivatePtr = ptr;

    return pOldPtr;
}

void* CEventEngine::GetPrivatePtr() const
{
    return m_pPrivatePtr;
}
