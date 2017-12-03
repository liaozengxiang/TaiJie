/*
 * 文件: Mutex.h
 * 功能: CMutex 类头文件
 * 作者: 廖增祥
 * 日期: 2013-04-26
 */

#ifndef ___MUTEX__H__20130426___
#define ___MUTEX__H__20130426___

#include "OSType.h"

#ifdef OS_LINUX
#include <pthread.h>
#endif

// 互斥锁类
class CMutex
{
public:
	CMutex();
	~CMutex();

	void Lock();
	void Unlock();
#ifdef OS_LINUX
	Bool TryLock();
#endif

protected:
#ifdef OS_WINDOWS
	CRITICAL_SECTION		m_internalLock;
#endif

#ifdef OS_LINUX
	pthread_mutex_t			m_internalLock;
#endif
};


// 自动互斥锁类
class CMutexLocker
{
public:
	CMutexLocker(CMutex *lpMutex);
	~CMutexLocker();

private:
	CMutexLocker(const CMutexLocker &rhs);
	CMutexLocker &operator =(const CMutexLocker &rhs);

protected:
	CMutex*					m_lpMutex;
};

#endif
