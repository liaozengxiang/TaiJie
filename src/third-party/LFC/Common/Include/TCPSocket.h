#ifndef ___TCP__SOCKET__20140428___
#define ___TCP__SOCKET__20140428___

#include "InetSocket.h"

class CTCPSocket : public CInetSocket
{
public:
	CTCPSocket();														// 构造一个无效的套接字对象
	CTCPSocket(Int32 nFD);												// 使用一个已有的套接字构造对象
	CTCPSocket(const CTCPSocket &rhs);									// 禁用拷贝构造函数，即不实现之
	virtual ~CTCPSocket();

public:
	// 重载 CSocket::Create 创建 TCP 套接字
	virtual Bool Create();

	Int32 Recv(LPSTR szBuff, Int32 nLen);
	Int32 Send(LPCSTR lpszBuff, Int32 nLen);
};

#endif
