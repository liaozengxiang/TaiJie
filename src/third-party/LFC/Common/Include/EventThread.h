#ifndef ___EVENT__THREAD__20150329___
#define ___EVENT__THREAD__20150329___

#include "EventEngine.h"
#include "Thread.h"

class CEventThread : public CEventEngine, public CThread
{
public:
    CEventThread();
    virtual ~CEventThread();

public:
    Bool Initialize(Int32 nCPUIndex = -1);

protected:
    // CEventEngine
    virtual void Loop(Int32 nTimeout);

protected:
    // CThread
    virtual Bool OnCreateThread(UInt32 &dwStackSize, Int32 &nCPUIndex);
    virtual void OnRun();

protected:
    Int32               m_nCPUIndex;
};

#endif
