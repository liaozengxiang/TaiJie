/*
 * 文件: AutoPtr.h
 * 功能: CAutoPtr 智能指针类定义
 * 作者: 廖增祥
 * 日期: 2013-04-29
 */

#ifndef ___AUTO__PTR__H__20130429___
#define ___AUTO__PTR__H__20130429___

#include "OSType.h"

template <typename T>
class CAutoPtr
{
public:
	explicit CAutoPtr(T *ptr) { m_ptr = ptr; }
	~CAutoPtr() { delete m_ptr; m_ptr = NULL; }

	// 释放对指针的所有权并返回该指针
	T* Release() { T *ptr = m_ptr; m_ptr = NULL; return ptr; }

	// 重载 * 和 -> 运算符
	T& operator *() { return *m_ptr; }
	T* operator ->() { return m_ptr; }

	// 返回指针
	T* Get() { return m_ptr; }

	// 重设指针，事先会删除原来拥有的指针
	void Reset(T *ptr) { if (m_ptr != ptr) { delete m_ptr; } m_ptr = ptr; }

private:
	// 禁用拷贝构造函数和赋值函数，即不实现之
	CAutoPtr(const CAutoPtr<T> &rhs);
	CAutoPtr<T>& operator =(const CAutoPtr<T> &rhs);

private:
	T*			m_ptr;
};

#endif
