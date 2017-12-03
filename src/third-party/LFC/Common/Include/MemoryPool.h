#ifndef ___MEMORY__POOL__20141227___
#define ___MEMORY__POOL__20141227___

#include "OSType.h"
#include "Mutex.h"
#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define DECLARE_MEMORY_POOL(className) \
	private: \
		static CMemoryPool<className> m_memoryPool ## className; \
	public: \
		void* operator new(size_t size) \
		{ \
			return m_memoryPool ## className.Allocate(); \
		} \
		void operator delete(void *pBuffer) \
		{ \
			m_memoryPool ## className.Free(pBuffer); \
		} \
		void* operator new[](size_t size); \
		void operator delete[](void *pBuffer); \
		private:

#define IMPLEMENT_MEMORY_POOL(className) \
	CMemoryPool<className> className::m_memoryPool ## className(#className);

template <typename T>
struct SMemoryPoolNode
{
	T						obj;
	SMemoryPoolNode*		pPrev;
	SMemoryPoolNode*		pNext;
};

// 定义内存池块的大小
#define MEMORY_POOL_BLOCK_SIZE	1048576LL

template <typename T>
class CMemoryPool
{
public:
	CMemoryPool(LPCSTR lpszClassName);
	~CMemoryPool();

public:
	void* Allocate();
	void Free(void *pBuffer);

private:
	Bool NeedMoreBlock();

private:
	std::string						m_strClassName;
	size_t							m_nBlockSize;
	std::list<void *>				m_lstBlockList;

	SMemoryPoolNode<T>*				m_pFreeList;
	SMemoryPoolNode<T>*				m_pUsedList;
	CMutex							m_lock;
};

template <typename T>
CMemoryPool<T>::CMemoryPool(LPCSTR lpszClassName) : m_strClassName(lpszClassName)
{
	m_nBlockSize = MEMORY_POOL_BLOCK_SIZE;
	m_nBlockSize = m_nBlockSize / sizeof(SMemoryPoolNode<T>) * sizeof(SMemoryPoolNode<T>);
	if (sizeof(SMemoryPoolNode<T>) > m_nBlockSize)
	{
		m_nBlockSize = 10 * sizeof(SMemoryPoolNode<T>);
	}

	m_pFreeList = NULL;
	m_pUsedList = NULL;

	NeedMoreBlock();
}

template <typename T>
CMemoryPool<T>::~CMemoryPool()
{
	// 打印出未释放的内存
	if (m_pUsedList != NULL)
	{
		SMemoryPoolNode<T> *ptr = m_pUsedList;
		while (ptr)
		{
			fprintf(stderr, "class %s: Memory leak found at %p with %lu B\n", m_strClassName.c_str(), ptr, sizeof(T));
			ptr = ptr->pNext;
		}
	}

	for (std::list<void *>::const_iterator it = m_lstBlockList.begin(); it != m_lstBlockList.end(); ++it)
	{
		free(*it);
	}
}

template <typename T>
void* CMemoryPool<T>::Allocate()
{
	CMutexLocker lock(&m_lock);

	if (NULL == m_pFreeList)
	{
		if (!NeedMoreBlock())
		{
			return NULL;
		}
	}

	SMemoryPoolNode<T> *pNode = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;

	if (m_pFreeList != NULL)
	{
		m_pFreeList->pPrev = NULL;
	}

	SMemoryPoolNode<T> *pLastUsedHead = m_pUsedList;
	m_pUsedList = pNode;
	m_pUsedList->pNext = pLastUsedHead;
	if (pLastUsedHead != NULL)
	{
		pLastUsedHead->pPrev = m_pUsedList;
	}

	return &pNode->obj;
}

template <typename T>
void CMemoryPool<T>::Free(void *pBuffer)
{
	CMutexLocker lock(&m_lock);

	SMemoryPoolNode<T> *pNode = (SMemoryPoolNode<T> *)pBuffer;
	if (pNode->pPrev != NULL)
	{
		pNode->pPrev->pNext = pNode->pNext;
		
		if (pNode->pNext != NULL)
		{
			pNode->pNext->pPrev = pNode->pPrev;
		}
	}
	else
	{
		m_pUsedList = pNode->pNext;
		if (m_pUsedList != NULL)
		{
			m_pUsedList->pPrev = NULL;
		}
	}

	pNode->pPrev = NULL;
	SMemoryPoolNode<T> *pLastFreeHead = m_pFreeList;
	m_pFreeList = pNode;
	m_pFreeList->pNext = pLastFreeHead;
	if (pLastFreeHead != NULL)
	{
		pLastFreeHead->pPrev = m_pFreeList;
	}
}

template <typename T>
Bool CMemoryPool<T>::NeedMoreBlock()
{
	void *ptr = malloc(m_nBlockSize);
	if (NULL == ptr)
	{
		return False;
	}

	Int32 nNodeCount = m_nBlockSize / sizeof(SMemoryPoolNode<T>);
	SMemoryPoolNode<T> *pLastNode = (SMemoryPoolNode<T> *)ptr;

	m_pFreeList = pLastNode;
	pLastNode->pPrev = NULL;
	pLastNode->pNext = NULL;

	for (Int32 i = 1; i < nNodeCount; ++i)
	{
		SMemoryPoolNode<T> *pNext = (SMemoryPoolNode<T> *)ptr + i;
		pNext->pPrev = pLastNode;
		pNext->pNext = NULL;
		pLastNode->pNext = pNext;
		pLastNode = pNext;
	}

	m_lstBlockList.push_back(ptr);
	return True;
}

#endif
