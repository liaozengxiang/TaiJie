#include "../Include/EventEngine.h"
#include "../Include/Event.h"
#include "../Include/Assert.h"
#include "../Include/Macro.h"
#include <iostream>

CEvent::CEvent(CEventEngine *lpEngine)
{
	m_lpEngine = lpEngine;
	m_bAdded   = False;

	AtomicInit(&m_nAtomicLock);
}

CEvent::~CEvent()
{
	Assert(m_bAdded == False);
}

Bool CEvent::RegisterRead()
{
	Bool bSuccess = False;

	do
	{
		AtomicLock(&m_nAtomicLock);
		if (m_bAdded)
		{
			bSuccess = m_lpEngine->ModEvent(this, EVENT_TYPE::EVENT_READ);
			break;
		}

		if (!m_lpEngine->AddEvent(this, EVENT_TYPE::EVENT_READ))
		{
			break;
		}

		m_bAdded = True;
		bSuccess = True;
	} while (0);

	AtomicUnlock(&m_nAtomicLock);
	return bSuccess;
}

Bool CEvent::RegisterWrite()
{
	Bool bSuccess = False;

	do
	{
		AtomicLock(&m_nAtomicLock);
		if (m_bAdded)
		{
			bSuccess = m_lpEngine->ModEvent(this, EVENT_TYPE::EVENT_WRITE);
			break;
		}

		if (!m_lpEngine->AddEvent(this, EVENT_TYPE::EVENT_WRITE))
		{
			break;
		}

		m_bAdded = True;
		bSuccess = True;
	} while (0);

	AtomicUnlock(&m_nAtomicLock);
	return bSuccess;
}

Bool CEvent::RegisterRW()
{
	Bool bSuccess = False;

	do
	{
		AtomicLock(&m_nAtomicLock);
		if (m_bAdded)
		{
			bSuccess = m_lpEngine->ModEvent(this, EVENT_TYPE::EVENT_READ|EVENT_TYPE::EVENT_WRITE);
			break;
		}

		if (!m_lpEngine->AddEvent(this, EVENT_TYPE::EVENT_READ|EVENT_TYPE::EVENT_WRITE))
		{
			break;
		}

		m_bAdded = True;
		bSuccess = True;
	} while (0);

	AtomicUnlock(&m_nAtomicLock);
	return bSuccess;
}

Bool CEvent::Unregister()
{
	Bool bSuccess = False;

	do
	{
		AtomicLock(&m_nAtomicLock);
		if (!m_bAdded)
		{
			break;
		}

		if (!m_lpEngine->DelEvent(this))
		{
			break;
		}

		m_bAdded = False;
		bSuccess = True;
	} while (0);

	AtomicUnlock(&m_nAtomicLock);
	return bSuccess;
}

void CEvent::OnRead()
{
}

void CEvent::OnWrite()
{
}

void CEvent::OnError(Int32 nErrCode)
{
	std::cerr << "Error occurred on socket " << GetFD() << ". ErrCode=" << nErrCode << ",this=" << this << std::endl;
}
