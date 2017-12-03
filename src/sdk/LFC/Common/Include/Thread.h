/*
 * Thread.h
 *
 *  Created on: Jul 18, 2013
 *      Author: lory
 */

#ifndef ___THREAD__20130718___
#define ___THREAD__20130718___

#include "OSType.h"
#include "Semaphore.h"

#ifdef OS_LINUX
#include <pthread.h>
#endif

class CThread
{
public:
	CThread();
	virtual ~CThread();

public:
	Bool Start();
	void Stop();

protected:
	virtual Bool OnCreateThread(UInt32 &dwStackSize, Int32 &nCPUIndex);
	virtual void OnRun() = 0;

protected:
	Bool TryQuit();
#ifdef OS_LINUX
	static void* __ThreadProc(void *lpParameter);
#else
	static DWORD WINAPI __ThreadProc(void *lpParameter);
#endif

protected:
#ifdef OS_LINUX
	pthread_t			m_hThread;
#else
	HANDLE				m_hThread;
#endif
    Int32               m_nCPUIndex;
	CSemaphore			m_sem;
};

#endif
