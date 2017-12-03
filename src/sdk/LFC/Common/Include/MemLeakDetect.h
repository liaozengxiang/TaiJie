#ifndef ___MEM__LEAK__DETECT__20140507___
#define ___MEM__LEAK__DETECT__20140507___

#ifdef LEAK

#include <map>
#include "OSType.h"
#include "Mutex.h"

class CMemLeakDetect
{
public:
	CMemLeakDetect();
	~CMemLeakDetect();

	void New(void *ptr, const std::string &strDesc);
	Bool Delete(void *ptr);
	void Print();

private:
	std::map<void *, std::string>						m_mapUsedMems;
	CMutex												m_mtxLock;
	UInt32												m_nLastThreadID;
};

void *operator new(size_t nSize);
void *operator new(size_t nSize, LPCSTR lpszFile, Int32 nLine);
void *operator new[](size_t nSize, LPCSTR lpszFile, Int32 nLine);
void operator delete(void *ptr);
void operator delete[](void *ptr);

#define new new(__FILE__, __LINE__)

#endif

#endif
