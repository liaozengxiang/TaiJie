#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include "../Include/MemLeakDetect.h"
#include "../Include/Assert.h"

#ifdef LEAK

// 定义静态的内存泄漏检测对象
static CMemLeakDetect s_memLeakDetect;

// 为了实现重载的 operator new, 必须解除对 new 的宏定义
#undef new

CMemLeakDetect::CMemLeakDetect()
{
	m_nLastThreadID = 0;
}

CMemLeakDetect::~CMemLeakDetect()
{
	Print();
	Assert(m_mapUsedMems.empty());
}

void CMemLeakDetect::New(void *ptr, const std::string &strDesc)
{
	UInt32 nCurThreadID = pthread_self();
	if (m_nLastThreadID != nCurThreadID)
	{
		m_mtxLock.Lock();

		m_nLastThreadID = nCurThreadID;
		m_mapUsedMems[ptr] = strDesc;
		m_nLastThreadID = 0;

		m_mtxLock.Unlock();
	}
	else
	{
		m_mapUsedMems[ptr] = strDesc;
	}
}

Bool CMemLeakDetect::Delete(void *ptr)
{
	UInt32 nCurThreadID = pthread_self();
	if (m_nLastThreadID != nCurThreadID)
	{
		CMutexLocker lock(&m_mtxLock);
		m_nLastThreadID = nCurThreadID;
		std::map<void *, std::string>::iterator it = m_mapUsedMems.find(ptr);
		if (it == m_mapUsedMems.end())
		{
			m_nLastThreadID = 0;			
			return False;
		}
		
		m_mapUsedMems.erase(it);
		return True;
	}

	std::map<void *, std::string>::iterator it = m_mapUsedMems.find(ptr);
	if (it == m_mapUsedMems.end())
	{
		return False;
	}

	m_mapUsedMems.erase(it);
	return True;
}

void CMemLeakDetect::Print()
{
	std::map<void *, std::string>::iterator it = m_mapUsedMems.begin();

	for (; it != m_mapUsedMems.end(); ++ it)
	{
		std::cerr << "Memory address: " << it->first << "\t" << it->second << std::endl;
	}
}

void *operator new(size_t nSize)
{
	void *ptr = malloc(nSize);
	if (NULL == ptr)
	{
		return ptr;
	}

	// 在这里不能再调用任何可能导致间接调用 operator new 的操作了
	//   否则会由于函数递归调用导致堆栈溢出
	return ptr;
}

void *operator new(size_t nSize, LPCSTR lpszFile, Int32 nLine)
{
	void *ptr = malloc(nSize);
	if (NULL == ptr)
	{
		return ptr;
	}

	Char szBytes[64];
#ifdef OS_64
	sprintf(szBytes, "%8lu B\t", nSize);
#else
	sprintf(szBytes, "%8u B\t", nSize);
#endif

	std::ostringstream oss;
	oss << szBytes << lpszFile << ":" << nLine;
	s_memLeakDetect.New(ptr, oss.str());

	return ptr;
}

void *operator new[](size_t nSize, LPCSTR lpszFile, Int32 nLine)
{
	return operator new(nSize, lpszFile, nLine);
}

void operator delete(void *ptr)
{
	if (ptr)
	{
		s_memLeakDetect.Delete(ptr);
		free(ptr);
	}
}

void operator delete[](void *ptr)
{
	operator delete(ptr);
}

#endif
