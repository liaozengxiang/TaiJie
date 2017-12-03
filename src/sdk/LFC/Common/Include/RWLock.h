#ifndef ___RWLOCK__H__20140411___
#define ___RWLOCK__H__20140411___

#include "OSType.h"
#include <pthread.h>

class CRWLock
{
public:
	CRWLock();
	~CRWLock();

	Bool RDLock();
	Bool WRLock();
	Bool Unlock();

private:
	pthread_rwlock_t	m_lock;
};

class CAutoReadLocker
{
public:
	CAutoReadLocker(CRWLock *pLock);
	~CAutoReadLocker();

private:
	CRWLock*			m_pLock;
};

class CAutoWriteLocker
{
public:
	CAutoWriteLocker(CRWLock *pLock);
	~CAutoWriteLocker();

private:
	CRWLock*			m_pLock;
};

#endif
