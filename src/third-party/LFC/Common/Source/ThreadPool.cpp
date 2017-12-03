#include "../Include/ThreadPool.h"
#include "../Include/Assert.h"


CThreadPool::CThreadPool()
{
	m_nEngineNum = 0;
}

CThreadPool::~CThreadPool()
{
	Stop();
}

Bool CThreadPool::Start(Int32 nThreadNum, UInt64 qwCPUMask)
{
	Assert(nThreadNum > 0);

	// 将 qwCPUMask 转换成 vector, 将其中的 1 的索引取出来
	std::vector<Int32> vecCPUIndex;
	for (Int32 nIndex = 0; (nIndex < 64) && (qwCPUMask != 0); ++nIndex)
	{
		if (qwCPUMask & 0x01)
		{
			vecCPUIndex.push_back(nIndex);
		}

		qwCPUMask >>= 1;
	}

	return Start(nThreadNum, vecCPUIndex);
}

Bool CThreadPool::Start(Int32 nThreadNum, const std::vector<Int32> &vecCPUIndex)
{
	Bool bSuccess = True;

	Assert(nThreadNum > 0);
	m_vecEngine.reserve(nThreadNum);
	m_vecEnginePayload.reserve(nThreadNum);
	m_nEngineNum = nThreadNum;

	Int32 nCPUIndex = -1;
	Int32 nLoopIndex = 0;

	for (Int32 i = 0; i < nThreadNum; ++i)
	{
		if (!vecCPUIndex.empty())
		{
			nCPUIndex = vecCPUIndex[nLoopIndex % vecCPUIndex.size()];
			++nLoopIndex;
		}

		CEventThread* pEngine = new CEventThread();
		if (!pEngine->Initialize(nCPUIndex))
		{
			delete pEngine;
			bSuccess = False;
			break;
		}

		m_vecEngine.push_back(pEngine);
		m_vecEnginePayload.push_back(0);
	}

	if (bSuccess)
	{
		Assert(m_nEngineNum == (Int32)m_vecEnginePayload.size());
		Assert(m_nEngineNum == (Int32)m_vecEngine.size());
	}

	return bSuccess;
}

void CThreadPool::Stop()
{
	std::vector<CEventEngine*>::const_iterator it = m_vecEngine.begin();
	for (; it != m_vecEngine.end(); ++it)
	{
		delete (*it);
	}
	m_vecEngine.clear();
	m_vecEnginePayload.clear();
}

Bool CThreadPool::AddTask(IWorkTask *pTask)
{
	// 选择一个工作线程
	Int32 nIndex = SelectEngineIndex();
	Assert(nIndex >= 0 && nIndex < m_nEngineNum);

	CEventEngine *pEngine = m_vecEngine[nIndex];
	Assert(pEngine != NULL);

	// 为该工作线程增加任务计数
	__sync_add_and_fetch(&m_vecEnginePayload[nIndex], 1);

	// 将任务投递给该线程
	if (!pEngine->PostMessage(nIndex, pTask, NULL, this))
	{
		__sync_sub_and_fetch(&m_vecEnginePayload[nIndex], 1);
		return False;
	}

	return True;
}

void CThreadPool::OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam)
{
	Int32 nThreadID = nMessageID;
	IWorkTask *pTask = (IWorkTask *)wParam;

	Assert(pTask != NULL);
	pTask->OnWork();

	// 为该工作线程递减任务计数
	__sync_sub_and_fetch(&m_vecEnginePayload[nThreadID], 1);
}

Int32 CThreadPool::SelectEngineIndex() const
{
	Int32 nPayload = 0x7FFFFFFF;
	Int32 nIndex = -1;

	for (Int32 i = 0; i < m_nEngineNum; ++i)
	{
		if (m_vecEnginePayload[i] < nPayload)
		{
			nPayload = m_vecEnginePayload[i];
			Assert(nPayload >= 0);

			nIndex = i;
		}
	}

	Assert(nIndex >= 0);
	return nIndex;
}
