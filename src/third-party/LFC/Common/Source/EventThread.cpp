#include <stdio.h>
#include "../Include/EventThread.h"
#include "../Include/Assert.h"

CEventThread::CEventThread()
{
    m_nCPUIndex = -1;
}

CEventThread::~CEventThread()
{
    Stop();
}

Bool CEventThread::Initialize(Int32 nCPUIndex)
{
    if (!CEventEngine::Initialize())
    {
        return False;
    }

    m_nCPUIndex = nCPUIndex;
    return Start();
}

Bool CEventThread::OnCreateThread(UInt32 &dwStackSize, Int32 &nCPUIndex)
{
    nCPUIndex = m_nCPUIndex;
    return CThread::OnCreateThread(dwStackSize, nCPUIndex);
}

void CEventThread::OnRun()
{
    while (!TryQuit())
    {
        EventTest(100);
    }
}

void CEventThread::Loop(Int32 nTimeout)
{
    fprintf(stderr, "CEventThread::Loop should not be called\n");
    Assert(False);
}
