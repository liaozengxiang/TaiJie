#ifndef ___SOCKET__H__20140426___
#define ___SOCKET__H__20140426___

#include "OSType.h"
#include "SocketAddr.h"

class CSocket
{
public:
	CSocket();																		// 构造一个无效的套接字对象
	CSocket(Int32 nFD);																// 用一个已有的文件描述符构造套接字对象
	CSocket(const CSocket &rhs);													// 禁用拷贝构造函数，不实现之
	virtual ~CSocket();

	virtual Bool Create() = 0;														// 创建一个套接字
	void  Attach(Int32 nFD);														// 维系一个文件描述符
	void  Detach();																	// 放弃对文件描述符的引用，不关闭套接字
	void  Close();																	// 关闭套接字
	Int32 GetFD() const;

	Bool SetNonblock(Bool bNonblock = True);

protected:
	Int32					m_nFD;
};

#endif
