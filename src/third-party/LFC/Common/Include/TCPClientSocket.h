/*
 * TCPClientSocket.h
 *
 *  Created on: Jun 13, 2016
 *      Author: lory
 */

#ifndef ___TCP__CLIENT__SOCKET__20160613___
#define ___TCP__CLIENT__SOCKET__20160613___

#include "TCPSocket.h"
#include "Event.h"
#include "Timer.h"

class CTCPClientSocket;
struct ITCPClientSocketEvent
{
    virtual void OnConnected(CTCPClientSocket *pSocket)                   = 0;
    virtual void OnConnectFailed(CTCPClientSocket *pSocket, Int32 nError) = 0;
    virtual void OnConnectTimeout(CTCPClientSocket *pSocket)              = 0;
};

class CTCPClientSocket : public CTCPSocket, public CEvent, public ITimerEvent
{
public:
    CTCPClientSocket(CEventEngine *pEngine);                    // 构造一个无效的套接字对象
	CTCPClientSocket(CEventEngine *pEngine, Int32 nFD);         // 使用一个已有的套接字构造对象
	CTCPClientSocket(const CTCPClientSocket &rhs);              // 禁用拷贝构造函数，即不实现之
	virtual ~CTCPClientSocket();

public:
    Bool Connect(const CSocketAddr &addr, ITCPClientSocketEvent *pCallback, Int32 nTimeout = -1);

public:
    virtual Int32 GetFD() const;
	
protected:
	virtual void  OnWrite();
	virtual void  OnError(Int32 nErrCode);

    // ITimerEvent
    virtual void OnTimer(CTimer *pTimer);

protected:
    CTimer*                 m_pTimer;
    ITCPClientSocketEvent*  m_pCallback;
};

#endif
