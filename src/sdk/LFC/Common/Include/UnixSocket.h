#ifndef ___UNIX__SOCKET__20150203___
#define ___UNIX__SOCKET__20150203___

#include "Socket.h"
#include <string>

class CUnixSocket : public CSocket
{
public:
	CUnixSocket();
	CUnixSocket(Int32 nFD);
	CUnixSocket(const CUnixSocket &rhs);
	virtual ~CUnixSocket();

public:
	virtual Bool Create();
	Bool  Connect(LPCSTR lpszUnixPath);
	Int32 Recv(LPSTR szBuff, Int32 nLen);
	Int32 Send(LPCSTR lpszBuff, Int32 nLen);
};

#endif
