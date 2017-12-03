#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "../Include/SocketUtil.h"

Bool SocketUtil::Initialize()
{
#ifdef OS_WINDOWS
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return False;
	}
	
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup();
		return False;
	}
	
	/* The WinSock DLL is acceptable. Proceed. */
	return True;
#else
	return True;
#endif
}

Bool SocketUtil::SetNonblock(Int32 nFD, Bool bNonblock)
{
#ifdef OS_LINUX
	Int32 nFL = fcntl(nFD, F_GETFL, 0);
	Int32 flag;
	
	if (bNonblock)
	{
		flag = nFL | O_NONBLOCK;
	}
	else
	{
		flag = nFL & ~O_NONBLOCK;
	}
	
	return -1 == fcntl(nFD, F_SETFL, flag) ? False : True;
#else
	unsigned long ul = 1;
	return (SOCKET_ERROR == ioctlsocket(nFD, FIONBIO, (unsigned long *)&ul)) ? False : True;
#endif
}

Bool SocketUtil::ReuseAddress(Int32 nFD)
{
	Int32 nOptval = 1;
	if (setsockopt(nFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&nOptval , sizeof(Int32)) < 0)
	{
		return False;
	}
	
	return True;
}

/*
 | ----------------------------------------------------------------------------------------------------
 | 设置 TCP Keep-Alive 选项
 | ----------------------------------------------------------------------------------------------------
 |
 | 通过设置 TCP Keep-Alive 选项可以保证在链接空闲时，通过协议检测链接是否存活
 | 如网线被拨掉时，可通过该机制检测 TCP Keep-Alive 超时后套接字上产生 110 超时错误，即可判断该连接已断开
 |
 | 参数: 
 |      nFD                 被设置的套接字
 |      nKeepIdle           TCP 连接在该空闲时段内没有收发数据时便主动产生 TCP Keep-Alive 请求
 |      nKeepInterval       发送 TCP Keep-Alive 请求的间隔
 |      nKeepCount          连续未收到 TCP Keep-Alive 响应的最大次数，即该连接超时的次数上限
 */
Bool SocketUtil::SetKeepAliveOption(Int32 nFD, Int32 nKeepIdle, Int32 nKeepInterval, Int32 nKeepCount)
{
    Bool bSuccess = False;

    do
    {
        // Keep-Alive 选项的开关
        Int32 nKeepAlive = 1;

        if (-1 == setsockopt(nFD, SOL_SOCKET, SO_KEEPALIVE, (char *)&nKeepAlive, sizeof(Int32)))
        {
            break;
        }

        if (-1 == setsockopt(nFD, SOL_TCP, TCP_KEEPIDLE, (char *)&nKeepIdle, sizeof(Int32)))
        {
            break;
        }

        if (-1 == setsockopt(nFD, SOL_TCP, TCP_KEEPINTVL, (char *)&nKeepInterval, sizeof(Int32)))
        {
            break;
        }

        if (-1 == setsockopt(nFD, SOL_TCP, TCP_KEEPCNT, (char *)&nKeepCount, sizeof(Int32)))
        {
            break;
        }

        bSuccess = True;
    } while (0);

    return bSuccess;
}

Bool SocketUtil::CloseKeepAliveOption(Int32 nFD)
{
    Int32 nKeepAlive = 0;
    return (setsockopt(nFD, SOL_SOCKET, SO_KEEPALIVE, (char *)&nKeepAlive, sizeof(Int32)) == 0);
}

Bool SocketUtil::SetCloseOnExec(Int32 nFD)
{
    Int32 flag = fcntl(nFD, F_GETFL, 0);
    flag |= FD_CLOEXEC;

    return (fcntl(nFD, F_SETFL, flag) != -1);
}

Int32 SocketUtil::Send(Int32 nFD, LPCSTR lpszBuff, Int32 nLen)
{
#ifdef OS_WINDOWS
	return send(nFD, lpszBuff, nLen, 0);
#else
	Int32 nSendSize;
	do
	{
		nSendSize = send(nFD, lpszBuff, nLen, 0);
	} while (nSendSize == -1 && errno == EINTR);

	return nSendSize;
#endif
}

Int32 SocketUtil::Recv(Int32 nFD, LPSTR szBuff, Int32 nLen)
{
#ifdef OS_WINDOWS
	return recv(nFD, szBuff, nLen, 0);
#else
	Int32 nRecvSize;
	do 
	{
		nRecvSize = recv(nFD, szBuff, nLen, 0);
	} while (nRecvSize == -1 && errno == EINTR);

	return nRecvSize;
#endif
}

Int32 SocketUtil::SendTo(Int32 nFD, LPCSTR lpszBuff, Int32 nLen, const CSocketAddr &address)
{
	struct sockaddr_in addrRemote = address.GetSocketAddr();
	return sendto(nFD, lpszBuff, nLen, 0, (struct sockaddr *)&addrRemote, sizeof(struct sockaddr));
}

Int32 SocketUtil::RecvFrom(Int32 nFD, LPSTR szBuff, Int32 nLen, CSocketAddr &address)
{
#ifdef OS_WINDOWS
	Int32 nSLen = sizeof(struct sockaddr);
#else
	socklen_t nSLen = sizeof(struct sockaddr);
#endif

	sockaddr_in addrFrom;
	Int32 nRecvSize = recvfrom(nFD, szBuff, nLen, 0, (struct sockaddr *)&addrFrom, &nSLen);
	if (nRecvSize > 0)
	{
		address.SetAddress(addrFrom);
	}

	return nRecvSize;
}

CSocketAddr SocketUtil::GetLocalAddress(Int32 nFD)
{
	struct sockaddr address = {0};
#ifdef OS_LINUX
	socklen_t nLen = sizeof(struct sockaddr);
#else
	Int32 nLen = sizeof(struct sockaddr);
#endif
	getsockname(nFD, &address, &nLen);
	
	return address;
}

CSocketAddr SocketUtil::GetRemoteAddress(Int32 nFD)
{
	struct sockaddr address = {0};
#ifdef OS_LINUX
	socklen_t nLen = sizeof(struct sockaddr);
#else
	Int32 nLen = sizeof(struct sockaddr);
#endif
	getpeername(nFD, &address, &nLen);
	
	return address;
}
