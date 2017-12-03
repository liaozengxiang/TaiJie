#ifndef ___REFERENCE__20150531___
#define ___REFERENCE__20150531___

#include "OSType.h"
#include "Assert.h"

class CReference
{
public:
	CReference() { m_lRefCount = 1; }
	virtual ~CReference() { Assert(m_lRefCount == 0); }

	Int64 AddRef() { return __sync_add_and_fetch(&m_lRefCount, 1); }
	Int64 Release() { Int64 lRefCount = __sync_sub_and_fetch(&m_lRefCount, 1); Assert(lRefCount >= 0); if (lRefCount == 0) { delete this; } return lRefCount; }
	Int64 GetRef() const { return m_lRefCount; }

protected:
	Int64 m_lRefCount;
};

class CNonDeleteReference
{
public:
	CNonDeleteReference() { m_lRefCount = 1; }
	virtual ~CNonDeleteReference() { Assert(m_lRefCount == 0); }
	
	Int64 AddRef() { return __sync_add_and_fetch(&m_lRefCount, 1); }
	Int64 Release() { Int64 lRefCount = __sync_sub_and_fetch(&m_lRefCount, 1); Assert(lRefCount >= 0); return lRefCount; }
	Int64 GetRef() const { return m_lRefCount; }
	
protected:
	Int64 m_lRefCount;
};

#endif
