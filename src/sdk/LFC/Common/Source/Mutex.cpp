#include "../Include/Mutex.h"
#include "../Include/Assert.h"

CMutex::CMutex()
{
#ifdef OS_WINDOWS
	InitializeCriticalSection(&m_internalLock);
#endif
	
#ifdef OS_LINUX
	pthread_mutex_init(&m_internalLock, NULL);
#endif
}

CMutex::~CMutex()
{
#ifdef OS_WINDOWS
	DeleteCriticalSection(&m_internalLock);
#endif
	
#ifdef OS_LINUX
	pthread_mutex_destroy(&m_internalLock);
#endif
}

void CMutex::Lock()
{
#ifdef OS_WINDOWS
	EnterCriticalSection(&m_internalLock);
#endif
	
#ifdef OS_LINUX
	pthread_mutex_lock(&m_internalLock);
#endif
}

void CMutex::Unlock()
{
#ifdef OS_WINDOWS
	LeaveCriticalSection(&m_internalLock);
#endif
	
#ifdef OS_LINUX
	pthread_mutex_unlock(&m_internalLock);
#endif
}

#ifdef OS_LINUX
Bool CMutex::TryLock()
{
	if (0 == pthread_mutex_trylock(&m_internalLock))
	{
		return True;
	}
	
	return False;
}
#endif


CMutexLocker::CMutexLocker(CMutex *lpMutex)
{
	Assert(lpMutex != NULL);
	m_lpMutex = lpMutex;

	m_lpMutex->Lock();
}

CMutexLocker::~CMutexLocker()
{
	m_lpMutex->Unlock();
}
