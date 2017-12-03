#include "../Include/UDPSocket.h"
#include "../Include/SocketUtil.h"

CUDPSocket::CUDPSocket()
{
}

CUDPSocket::CUDPSocket(Int32 nFD) : CInetSocket(nFD)
{
}

CUDPSocket::~CUDPSocket()
{
}

Bool CUDPSocket::Create()
{
	m_nFD = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
	return (m_nFD != -1);
}

Int32 CUDPSocket::RecvFrom(LPSTR szBuff, Int32 nLen, CSocketAddr &address)
{
	return SocketUtil::RecvFrom(m_nFD, szBuff, nLen, address);
}

Int32 CUDPSocket::SendTo(LPCSTR lpszBuff, Int32 nLen, const CSocketAddr &address)
{
	return SocketUtil::SendTo(m_nFD, lpszBuff, nLen, address);
}
