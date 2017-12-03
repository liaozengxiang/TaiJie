#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "../Include/SocketAddr.h"

CSocketAddr::CSocketAddr()
{
	memset(&m_sockAddr, 0, sizeof(struct sockaddr));
}

CSocketAddr::CSocketAddr(const char *lpszIP, UInt16 wPort)
{
	SetAddress(lpszIP, wPort);
}

CSocketAddr::CSocketAddr(const char *lpszAddress)
{
	SetAddress(lpszAddress);
}

CSocketAddr::CSocketAddr(const struct sockaddr &address)
{
	memcpy(&m_sockAddr, &address, sizeof(struct sockaddr));
}

CSocketAddr::CSocketAddr(const struct sockaddr_in &address)
{
	m_sockAddr = address;
}

void CSocketAddr::SetAddress(const char *lpszIP, UInt16 wPort)
{
	m_sockAddr.sin_port        = htons(wPort);
	m_sockAddr.sin_family      = AF_INET;
	m_sockAddr.sin_addr.s_addr = inet_addr(lpszIP);
}

void CSocketAddr::SetAddress(const char *lpszAddress)
{
	int a1, a2, a3, a4, nPort;
	sscanf(lpszAddress, "%d.%d.%d.%d:%d", &a1, &a2, &a3, &a4, &nPort);
	
	char szIP[32];
	sprintf(szIP, "%d.%d.%d.%d", a1, a2, a3, a4);
	
	SetAddress(szIP, (UInt16)nPort);
}

void CSocketAddr::SetAddress(const struct sockaddr &address)
{
	memcpy(&m_sockAddr, &address, sizeof(struct sockaddr));
}

void CSocketAddr::SetAddress(const struct sockaddr_in &address)
{
	m_sockAddr = address;
}

struct sockaddr_in CSocketAddr::GetSocketAddr() const
{
	return m_sockAddr;
}

std::string CSocketAddr::GetIPString() const
{
	return inet_ntoa(m_sockAddr.sin_addr);
}

UInt16 CSocketAddr::GetPort() const
{
	return htons(m_sockAddr.sin_port);
}

std::string CSocketAddr::GetAddrString() const
{
	char szAddr[32];
	sprintf(szAddr, "%s:%d", inet_ntoa(m_sockAddr.sin_addr), htons(m_sockAddr.sin_port));
	return szAddr;
}
