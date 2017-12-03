#ifndef ___EVENT__20140422___
#define ___EVENT__20140422___

#include "OSType.h"

// 定义 I/O 事件类型
namespace EVENT_TYPE
{
	enum _EVENT_TYPE
	{
		EVENT_READ  = 0x01,
		EVENT_WRITE = 0x02
	};
}

class CEventEngine;
class CEvent
{
public:
	CEvent(CEventEngine *lpEngine);
	virtual ~CEvent();

	virtual Int32 GetFD() const = 0;                          // 子类需要实现该函数返回实际的文件句柄
	virtual void  OnRead();                                   // 当文件描述符有读事件时被调用
	virtual void  OnWrite();                                  // 当文件描述符有写事件时被调用
	virtual void  OnError(Int32 nErrCode);                    // 当文件描述符有错误时被调用

protected:
	Bool RegisterRead();
	Bool RegisterWrite();
	Bool RegisterRW();
	Bool Unregister();

protected:
	CEventEngine*					m_lpEngine;
	Bool							m_bAdded;                 // 标识文件描述符是否已被加入监听
	Int32							m_nAtomicLock;            // 用于实现原子访问的标识
};

#endif
