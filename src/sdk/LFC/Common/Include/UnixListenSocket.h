#ifndef ___UNIX__LISTEN__SOCKET__20150204___
#define ___UNIX__LISTEN__SOCKET__20150204___

#include "UnixSocket.h"
#include <string>

class CUnixListenSocket : public CUnixSocket
{
public:
	CUnixListenSocket();
	CUnixListenSocket(Int32 nFD);
	CUnixListenSocket(const CUnixListenSocket &rhs);
	virtual ~CUnixListenSocket();

public:
	Bool  Bind(LPCSTR lpszUnixPath);
	Bool  Listen(Int32 nBacklogSize = 128);
	Int32 Accept();

protected:
	std::string				m_strUnixPath;
};

#endif
