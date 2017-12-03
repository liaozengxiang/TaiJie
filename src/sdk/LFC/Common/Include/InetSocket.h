#ifndef ___INET__SOCKET__20150203___
#define ___INET__SOCKET__20150203___

#include "Socket.h"

class CInetSocket : public CSocket
{
public:
	CInetSocket();
	CInetSocket(Int32 nFD);
	CInetSocket(const CInetSocket &rhs);

public:
	Bool ReuseAddress();
	Bool Connect(const CSocketAddr &address);
	Bool Bind(const CSocketAddr &address);
};

#endif
