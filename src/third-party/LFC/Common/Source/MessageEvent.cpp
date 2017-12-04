#include "../Include/MessageEvent.h"
#include "../Include/Assert.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>

CMessageEvent::CMessageEvent(CEventEngine *pEngine) : CEvent(pEngine)
{
    m_nMessageID = 0;
    m_wParam     = NULL;
    m_lParam     = NULL;
    m_pMsgEvent  = NULL;
    m_nEventFD   = -1;
}

CMessageEvent::~CMessageEvent()
{
    Unregister();

    if (m_nEventFD != -1)
    {
        close(m_nEventFD);
        m_nEventFD = -1;
    }
}

Bool CMessageEvent::Create(Int32 nMessageID, void *wParam, void *lParam, IMessageEvent *pMsgEvent)
{
    // 存储消息参数
    m_nMessageID = nMessageID;
    m_wParam     = wParam;
    m_lParam     = lParam;
    m_pMsgEvent  = pMsgEvent;

    return TriggerEvent();
}

Bool CMessageEvent::Create(Int32 nMessageID, void *wParam, void *lParam, const CSharedPtr<IMessageEvent> &spCallback)
{
    // 存储消息参数
    m_nMessageID = nMessageID;
    m_wParam     = wParam;
    m_lParam     = lParam;
    m_spCallback = spCallback;

    return TriggerEvent();
}

Bool CMessageEvent::TriggerEvent()
{
    Assert(m_nEventFD == -1);
    m_nEventFD = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    if (-1 == m_nEventFD)
    {
        return False;
    }

    if (!RegisterRead())
    {
        return False;
    }

    // 向 eventfd 写入 8 字节整数的 1, 触发读事件
    Int64 lValue = 1;
    Int32 nWriteSize = write(m_nEventFD, (LPCSTR)&lValue, 8);
    if (nWriteSize != 8)
    {
        return False;
    }

    return True;
}

Int32 CMessageEvent::GetFD() const
{
    return m_nEventFD;
}

void CMessageEvent::OnRead()
{
    if (m_pMsgEvent != NULL)
    {
        m_pMsgEvent->OnMessageEvent(m_nMessageID, m_wParam, m_lParam);
    }
    else
    {
        m_spCallback->OnMessageEvent(m_nMessageID, m_wParam, m_lParam);
    }

    delete this;
}

void CMessageEvent::OnWrite()
{
    Assert(False);
}

void CMessageEvent::OnError(Int32 nErrCode)
{
    std::cerr << "Error happened on epoll. ErrCode=" << nErrCode << ",eventfd=" << m_nEventFD << std::endl;

    if (m_pMsgEvent != NULL)
    {
        m_pMsgEvent->OnMessageEvent(m_nMessageID, m_wParam, m_lParam);
    }
    else
    {
        m_spCallback->OnMessageEvent(m_nMessageID, m_wParam, m_lParam);
    }

    delete this;
}
