#ifndef ___SOCKET__UTIL__20140427___
#define ___SOCKET__UTIL__20140427___

#include "OSType.h"
#include "SocketAddr.h"

namespace SocketUtil
{
    Bool Initialize();														// 套接字库的初始化
    Bool SetNonblock(Int32 nFD, Bool bNonblock);
    Bool ReuseAddress(Int32 nFD);
    Bool SetKeepAliveOption(Int32 nFD, Int32 nKeepIdle, Int32 nKeepInterval, Int32 nKeepCount);
    Bool CloseKeepAliveOption(Int32 nFD);
    Bool SetCloseOnExec(Int32 nFD);

    Int32 Send(Int32 nFD, LPCSTR lpszBuff, Int32 nLen);
    Int32 Recv(Int32 nFD, LPSTR szBuff, Int32 nLen);
    Int32 RecvFrom(Int32 nFD, LPSTR szBuff, Int32 nLen, CSocketAddr &address);
    Int32 SendTo(Int32 nFD, LPCSTR lpszBuff, Int32 nLen, const CSocketAddr &address);

    CSocketAddr GetLocalAddress(Int32 nFD);
    CSocketAddr GetRemoteAddress(Int32 nFD);

    // 套接字缓冲区大小
    Int32 GetRecvBufferSize(Int32 nFD);
    Int32 GetSendBufferSize(Int32 nFD);
    Bool SetRecvBufferSize(Int32 nFD, Int32 nSize);
    Bool SetSendBufferSize(Int32 nFD, Int32 nSize);
}

#endif
