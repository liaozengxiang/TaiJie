#include "../Include/TCPSocket.h"
#include "../Include/SocketUtil.h"

CTCPSocket::CTCPSocket()
{
}

CTCPSocket::CTCPSocket(Int32 nFD) : CInetSocket(nFD)
{
}

CTCPSocket::~CTCPSocket()
{
}

Bool CTCPSocket::Create()
{
	m_nFD = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, 0);
	return (m_nFD != -1);
}

Int32 CTCPSocket::Recv(LPSTR szBuff, Int32 nLen)
{
	return SocketUtil::Recv(m_nFD, szBuff, nLen);
}

Int32 CTCPSocket::Send(LPCSTR lpszBuff, Int32 nLen)
{
	return SocketUtil::Send(m_nFD, lpszBuff, nLen);
}
