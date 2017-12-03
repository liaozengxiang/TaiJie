/*
 * ThreadPool.h
 *
 *  Created on: Mar 27, 2016
 *      Author: lory
 */

#ifndef THREAD__POOL__20160327
#define THREAD__POOL__20160327

#include "OSType.h"
#include "Thread.h"
#include "EventThread.h"
#include <vector>

struct IWorkTask
{
	virtual ~IWorkTask() {}
	virtual void OnWork() = 0;
};

class CThreadPool : public IMessageEvent
{
public:
	CThreadPool();
	~CThreadPool();

public:
	Bool Start(Int32 nThreadNum, UInt64 qwCPUMask = 0);
	Bool Start(Int32 nThreadNum, const std::vector<Int32> &vecCPUIndex);
	void Stop();
	Bool AddTask(IWorkTask *pTask);

private:
	// IMessageEvent
	virtual void OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam);

	// Helper function(s)
	Int32 SelectEngineIndex() const;											// 选择一个负载较轻的工作线程

private:
	std::vector<CEventEngine*>					m_vecEngine;					// 线程池
	std::vector<Int32>							m_vecEnginePayload;				// 存储每个线程的负载值
	Int32										m_nEngineNum;					// 线程的数量
};

#endif
