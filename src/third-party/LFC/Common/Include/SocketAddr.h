#ifndef ___SOCKET__ADDR__20140427___
#define ___SOCKET__ADDR__20140427___

#include <netinet/in.h>
#include <string>
#include "OSType.h"

class CSocketAddr
{
public:
	CSocketAddr();
	CSocketAddr(LPCSTR lpszIP, UInt16 wPort);
	CSocketAddr(LPCSTR lpszAddress);
	CSocketAddr(const struct sockaddr &address);
	CSocketAddr(const struct sockaddr_in &address);
	
public:
	void SetAddress(const char *lpszIP, UInt16 wPort);
	void SetAddress(const char *lpszAddress);
	void SetAddress(const struct sockaddr &address);
	void SetAddress(const struct sockaddr_in &address);
	
	struct sockaddr_in GetSocketAddr() const;
	std::string        GetIPString() const;
	UInt16             GetPort() const;
	std::string        GetAddrString() const;
	
protected:
	struct sockaddr_in				m_sockAddr;
};

#endif
