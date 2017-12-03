/*
 * Semaphore.cpp
 *
 *  Created on: Jul 20, 2013
 *      Author: lory
 */

#include "../Include/Semaphore.h"
#include "../Include/OS.h"
#include <time.h>

#ifdef OS_LINUX
#include <errno.h>
#endif

CSemaphore::CSemaphore(Int32 nInitial)
{
#ifdef OS_LINUX
	sem_init(&m_sem, 0, nInitial);
#else
	m_sem = CreateSemaphore(NULL, nInitial, 0x7FFFFFFF, NULL);
#endif
}

CSemaphore::~CSemaphore()
{
#ifdef OS_LINUX
	sem_destroy(&m_sem);
#else
	CloseHandle(m_sem);
	m_sem = NULL;
#endif
}

void CSemaphore::PostSignal()
{
#ifdef OS_LINUX
	sem_post(&m_sem);
#else
	ReleaseSemaphore(m_sem, 1, NULL);
#endif
}

Bool CSemaphore::TryWaitForSignal(Int32 nMillisecond)
{
	Bool bSuccess = False;

	if (0 == nMillisecond)
	{
#ifdef OS_LINUX
		if (0 == sem_trywait(&m_sem))
		{
			bSuccess = True;
		}
#else
		if (WAIT_OBJECT_0 == WaitForSingleObject(m_sem, 0))
		{
			bSuccess = True;
		}
#endif
	}
	else
	{
#ifdef OS_LINUX
		Int64 lTotalMillisec = OS::Milliseconds() + nMillisecond;
		struct timespec ts;

		ts.tv_sec = lTotalMillisec / 1000;
		ts.tv_nsec = (lTotalMillisec % 1000) * 1000000l;

		Int32 retval;
		while (1)
		{
			retval = sem_timedwait(&m_sem, &ts);
			if (-1 == retval && errno == EINTR)
			{
				continue;
			}

			break;
		}

		if (0 == retval)
		{
			bSuccess = True;
		}
#else
		if (WAIT_OBJECT_0 == WaitForSingleObject(m_sem, nMillisecond))
		{
			bSuccess = True;
		}
#endif
	}

	return bSuccess;
}

Bool CSemaphore::WaitForSignal()
{
#ifdef OS_LINUX
	while (-1 == sem_wait(&m_sem) && errno == EINTR)
	{
		continue;
	}
#else
	WaitForSingleObject(m_sem, INFINITE);
#endif

	return True;
}
