/*
 * Thread.cpp
 *
 *  Created on: Jul 18, 2013
 *      Author: lory
 */

#define __USE_GNU
#include <sched.h>
#include "../Include/Thread.h"

CThread::CThread()
{
	m_hThread = 0;
    m_nCPUIndex = -1;
}

CThread::~CThread()
{
	Stop();
}

Bool CThread::OnCreateThread(UInt32 &dwStackSize, Int32 &nCPUIndex)
{
	return True;
}

#ifdef OS_LINUX
void *CThread::__ThreadProc(void *lpParameter)
#else
DWORD WINAPI CThread::__ThreadProc(void *lpParameter)
#endif
{
	CThread *lpInstance = (CThread *)lpParameter;

#ifdef OS_LINUX
    // 如果用户指定了绑定 CPU, 则在此进行绑定
    if (lpInstance->m_nCPUIndex != -1)
    {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(lpInstance->m_nCPUIndex, &mask);
        sched_setaffinity(0, sizeof(mask), &mask);
    }
#endif

	lpInstance->OnRun();
	return NULL;
}

Bool CThread::Start()
{
	Bool bSuccess = False;

	do
	{
		UInt32 dwStackSize = 0;			// 默认使用与调用线程相同的堆栈大小
		if (!OnCreateThread(dwStackSize, m_nCPUIndex))
		{
			// 上层初始化线程函数返回失败
			break;
		}

#ifdef OS_LINUX
		pthread_attr_t  attr;
		pthread_attr_t* lpAttr = NULL;

		do
		{	// 设置线程堆栈大小
			if (dwStackSize <= 0)
			{
				break;
			}

			if (pthread_attr_init(&attr) != 0)
			{
				break;
			}

			if (pthread_attr_setstacksize(&attr, dwStackSize) != 0)
			{
				pthread_attr_destroy(&attr);
				break;
			}

			lpAttr = &attr;
		} while (0);

		// 创建线程
		if (0 != pthread_create(&m_hThread, lpAttr, __ThreadProc, this))
		{
			if (lpAttr)
			{
				pthread_attr_destroy(&attr);
			}
			break;
		}

		if (lpAttr)
		{
			pthread_attr_destroy(&attr);
		}
#else
		DWORD dwThreadID;
		m_hThread = CreateThread(NULL, dwStackSize, __ThreadProc, this, 0, &dwThreadID);
		if (NULL == m_hThread)
		{
			break;
		}
#endif

		bSuccess = True;
	} while (0);

	if (!bSuccess)
	{
		m_hThread = 0;
	}

	return bSuccess;
}

void CThread::Stop()
{
	if (m_hThread)
	{
		m_sem.PostSignal();

#ifdef OS_LINUX
		void *lpDummy = NULL;
		pthread_join(m_hThread, &lpDummy);
#else
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
#endif

		m_hThread = 0;
	}
}

Bool CThread::TryQuit()
{
	return m_sem.TryWaitForSignal(0);
}
