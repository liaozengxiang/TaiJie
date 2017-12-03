#include <sys/socket.h>
#include <unistd.h>
#include "../Include/Socket.h"
#include "../Include/SocketUtil.h"

CSocket::CSocket()
{
	m_nFD = -1;
}

CSocket::CSocket(Int32 nFD)
{
	m_nFD = nFD;
}

CSocket::~CSocket()
{
	Close();
}

void CSocket::Attach(Int32 nFD)
{
	Close();
	m_nFD = nFD;
}

void CSocket::Detach()
{
	m_nFD = -1;
}

void CSocket::Close()
{
	if (-1 != m_nFD)
	{
#ifdef OS_LINUX
		shutdown(m_nFD, SHUT_RDWR);
		close(m_nFD);
#else
		shutdown(m_nFD, SD_BOTH);
		closesocket(m_nFD);
#endif
		m_nFD = -1;
	}
}

Int32 CSocket::GetFD() const
{
	return m_nFD;
}

Bool CSocket::SetNonblock(Bool bNonblock)
{
	return SocketUtil::SetNonblock(m_nFD, bNonblock);
}
