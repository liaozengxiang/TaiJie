#include "../Include/InetListenSocket.h"

CInetListenSocket::CInetListenSocket()
{
}

CInetListenSocket::CInetListenSocket(Int32 nFD) : CTCPSocket(nFD)
{
}

CInetListenSocket::~CInetListenSocket()
{
}

Bool CInetListenSocket::Listen(Int32 nBacklogSize)
{
	return listen(m_nFD, nBacklogSize) == 0;
}

Int32 CInetListenSocket::Accept()
{
	sockaddr clientAddr;
#ifdef OS_WINDOWS
	Int32 nLen = sizeof(sockaddr_in);
#else
	socklen_t nLen = sizeof(sockaddr_in);
#endif
	return accept(m_nFD, &clientAddr, &nLen);
}

Int32 CInetListenSocket::Accept(CSocketAddr &addr)
{
	sockaddr clientAddr;
#ifdef OS_WINDOWS
	Int32 nLen = sizeof(sockaddr_in);
#else
	socklen_t nLen = sizeof(sockaddr_in);
#endif
	Int32 nFD = accept(m_nFD, &clientAddr, &nLen);
	addr.SetAddress(clientAddr);

	return nFD;
}
