#include "../Include/InetSocket.h"
#include "../Include/SocketUtil.h"

CInetSocket::CInetSocket()
{
}

CInetSocket::CInetSocket(Int32 nFD) : CSocket(nFD)
{
}

Bool CInetSocket::ReuseAddress()
{
	return SocketUtil::ReuseAddress(m_nFD);
}

Bool CInetSocket::Connect(const CSocketAddr &address)
{
	struct sockaddr_in addrRemote = address.GetSocketAddr();
	return connect(m_nFD, (struct sockaddr *)&addrRemote, sizeof(struct sockaddr)) == 0;
}

Bool CInetSocket::Bind(const CSocketAddr &address)
{
	struct sockaddr_in addrBind = address.GetSocketAddr();
	return bind(m_nFD, (struct sockaddr *)&addrBind, sizeof(struct sockaddr)) == 0;
}
