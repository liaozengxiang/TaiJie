#include "../Include/UnixListenSocket.h"
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

CUnixListenSocket::CUnixListenSocket()
{
}

CUnixListenSocket::CUnixListenSocket(Int32 nFD) : CUnixSocket(nFD)
{
}

CUnixListenSocket::~CUnixListenSocket()
{
	if (!m_strUnixPath.empty())
	{
		unlink(m_strUnixPath.c_str());
	}
}

Bool CUnixListenSocket::Bind(LPCSTR lpszUnixPath)
{
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, lpszUnixPath);

	std::string strUnixPath = lpszUnixPath;
	if (bind(m_nFD, (struct sockaddr *)&addr, sizeof(addr.sun_family) + strUnixPath.length()) != -1)
	{
		// 保存 Unix 路径名称
		m_strUnixPath = strUnixPath;
		return True;
	}

	return False;
}

Bool CUnixListenSocket::Listen(Int32 nBacklogSize)
{
	return listen(m_nFD, nBacklogSize) == 0;
}

Int32 CUnixListenSocket::Accept()
{
	struct sockaddr_un clientAddr;
	socklen_t nLen = sizeof(clientAddr.sun_family) + m_strUnixPath.length();
	return accept(m_nFD, (struct sockaddr *)&clientAddr, &nLen);
}
