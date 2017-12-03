#ifndef ___INET__LISTEN__SOCKET__20150204___
#define ___INET__LISTEN__SOCKET__20150204___

#include "TCPSocket.h"
#include "SocketAddr.h"

class CInetListenSocket : public CTCPSocket
{
public:
	CInetListenSocket();
	CInetListenSocket(Int32 nFD);
	CInetListenSocket(const CInetListenSocket &rhs);
	virtual ~CInetListenSocket();

public:
	Bool  Listen(Int32 nBacklogSize = 128);
	Int32 Accept();
	Int32 Accept(CSocketAddr &addr);
};

#endif
