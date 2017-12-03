/*
 * Semaphore.h
 *
 *  Created on: Jul 20, 2013
 *      Author: lory
 */

#ifndef ___SEMAPHORE__20130720___
#define ___SEMAPHORE__20130720___

#include "OSType.h"

#ifdef OS_LINUX
#include <semaphore.h>
#endif

class CSemaphore
{
public:
	CSemaphore(Int32 nInitial = 0);
	virtual ~CSemaphore();

public:
	void PostSignal();
	Bool TryWaitForSignal(Int32 nMillisecond = 0);
	Bool WaitForSignal();

protected:
#ifdef OS_LINUX
	sem_t			m_sem;
#else
	HANDLE			m_sem;
#endif
};

#endif
