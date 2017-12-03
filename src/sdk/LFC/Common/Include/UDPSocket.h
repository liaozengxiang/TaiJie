#ifndef ___UDP__SOCKET__20140428___
#define ___UDP__SOCKET__20140428___

#include "InetSocket.h"

class CUDPSocket : public CInetSocket
{
public:
	CUDPSocket();																	// 构造一个无效的套接字
	CUDPSocket(Int32 nFD);															// 使用一个已有的套接字构造对象
	CUDPSocket(const CUDPSocket &rhs);												// 禁用拷贝构造函数，即不实现之
	virtual ~CUDPSocket();

	// 重载 CSocket::Create 创建 UDP 套接字
	virtual Bool Create();
	
	Int32 RecvFrom(LPSTR szBuff, Int32 nLen, CSocketAddr &address);
	Int32 SendTo(LPCSTR lpszBuff, Int32 nLen, const CSocketAddr &address);
};

#endif
