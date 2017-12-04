#ifndef TCP__CONNECTOR__H__20170522
#define TCP__CONNECTOR__H__20170522

#include "OSType.h"
#include "SharedPtr.h"
#include "Timer.h"
#include "TCPSocket.h"

struct ITCPConnectorCallback
{
    virtual ~ITCPConnectorCallback() {}
    virtual void OnConnected()                 = 0;
    virtual void OnConnectFailed(Int32 nError) = 0;
    virtual void OnConnectTimeout()            = 0;
};

class CTCPConnector : public CEvent, public ITimerEvent
{
public:
    CTCPConnector(CEventEngine *pEngine);
    virtual ~CTCPConnector();

public:
    Bool Connect(const CSocketAddr &addr, CWeakPtr<ITCPConnectorCallback> wpCallback, Int32 nTimeout = -1);
    void Detach();

public:
    // CEvent
    virtual Int32 GetFD() const { return m_socket.GetFD(); }

protected:
    virtual void  OnRead() { }
    virtual void  OnWrite();
    virtual void  OnError(Int32 nErrCode);

    // ITimerEvent
    virtual void OnTimer(CTimer *pTimer);

protected:
    CTCPSocket                          m_socket;
    CTimer*                             m_pTimer;
    CWeakPtr<ITCPConnectorCallback>     m_wpCallback;
};

#endif
