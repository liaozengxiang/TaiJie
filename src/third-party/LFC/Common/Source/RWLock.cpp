#include "../Include/RWLock.h"
#include "../Include/Assert.h"

CRWLock::CRWLock()
{
	pthread_rwlock_init(&m_lock, NULL);
}

CRWLock::~CRWLock()
{
	pthread_rwlock_destroy(&m_lock);
}

Bool CRWLock::RDLock()
{
	return (pthread_rwlock_rdlock(&m_lock) == 0);
}

int CRWLock::WRLock()
{
	return (pthread_rwlock_wrlock(&m_lock) == 0);
}

int CRWLock::Unlock()
{
	return (pthread_rwlock_unlock(&m_lock) == 0);
}



CAutoReadLocker::CAutoReadLocker(CRWLock *pLock)
{
	Assert(pLock != NULL);
	m_pLock = pLock;

	pLock->RDLock();
}

CAutoReadLocker::~CAutoReadLocker()
{
	m_pLock->Unlock();
}


CAutoWriteLocker::CAutoWriteLocker(CRWLock *pLock)
{
	Assert(pLock != NULL);
	m_pLock = pLock;

	pLock->WRLock();
}

CAutoWriteLocker::~CAutoWriteLocker()
{
	m_pLock->Unlock();
}
