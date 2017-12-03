#include "../Include/UnixSocket.h"
#include "../Include/SocketUtil.h"
#include <string.h>
#include <sys/un.h>

CUnixSocket::CUnixSocket()
{
}

CUnixSocket::CUnixSocket(Int32 nFD) : CSocket(nFD)
{
}

CUnixSocket::~CUnixSocket()
{
}

Bool CUnixSocket::Create()
{
	m_nFD = socket(AF_UNIX, SOCK_STREAM, 0);
	return m_nFD != -1;
}

Bool CUnixSocket::Connect(LPCSTR lpszUnixPath)
{
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, lpszUnixPath);

	return (connect(m_nFD, (struct sockaddr *)&addr, sizeof(addr.sun_family) + strlen(lpszUnixPath)) != -1);
}

Int32 CUnixSocket::Recv(LPSTR szBuff, Int32 nLen)
{
	return SocketUtil::Recv(m_nFD, szBuff, nLen);
}

Int32 CUnixSocket::Send(LPCSTR lpszBuff, Int32 nLen)
{
	return SocketUtil::Send(m_nFD, lpszBuff, nLen);
}
