/*
 * 文件: SharedPtr.h
 * 功能: 引用计数型智能指针
 * 作者: 廖增祥
 * 日期: 2013-05-05
 * 警告: 在使用引用计数型智能指针时，千万不要将本类对象的指针保存在
 *       本类的成员变量中，也不可有任何形式的智能指针循环引用，否则
 *       此对象或多个对象将永远无法被释放
 */

#ifndef ___SHARED__PTR__H__20130505___
#define ___SHARED__PTR__H__20130505___

#include "OSType.h"

template <typename T>
class CSharedPtr
{
public:
	// 用一个堆指针构造智能指针对象
	explicit CSharedPtr(T *ptr = NULL)
	{
		m_pInstance = ptr;
		m_pRefCount = new Int64(1);
	}

	// 用一个智能指针构造对象
	CSharedPtr(const CSharedPtr &rhs)
	{
		m_pRefCount = rhs.m_pRefCount;
		m_pInstance = rhs.m_pInstance;
		__sync_add_and_fetch(m_pRefCount, 1);
	}
	
	// 智能指针的赋值，会先删除原来拥有的指针，再接管新的指针引用计数加1
	CSharedPtr& operator =(const CSharedPtr &rhs)
	{
		__Free();
		m_pRefCount = rhs.m_pRefCount;
		m_pInstance = rhs.m_pInstance;
		__sync_add_and_fetch(m_pRefCount, 1);

		return *this;
	}

	// 析构函数，解绑指针
	~CSharedPtr() { __Free(); }

	// 返回绑定的对象指针
	T* operator ->() { return m_pInstance; }

	// 返回绑定的对象引用
	T& operator *() { return *m_pInstance; }

	// 返回绑定的资源指针
	T* Get() { return m_pInstance; }

	// 判断智能指针和其他类型是否相等，实际上比较的是它所绑定的指针的值
	Bool operator ==(const CSharedPtr &rhs) { return m_pInstance == rhs.m_pInstance; }
	Bool operator ==(const void *ptr) { return m_pInstance == (const T *)ptr; }

	Bool operator !=(const CSharedPtr &rhs) { return m_pInstance != rhs.m_pInstance; }
	Bool operator !=(const void *ptr) { return m_pInstance != (const T *)ptr; }

private:
	void __Free()
	{
		// 计数器减 1
		if (__sync_sub_and_fetch(m_pRefCount, 1) == 0)
		{
			delete m_pInstance;
			m_pInstance = NULL;

			delete m_pRefCount;
			m_pRefCount = NULL;
		}
	}

private:
	Int64*		m_pRefCount;				// 引用计数器
	T*			m_pInstance;				// 被绑定的实例指针
};

#endif
