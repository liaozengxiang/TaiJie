#include <sys/timerfd.h>
#include <unistd.h>
#include <fcntl.h>
#include "../Include/Timer.h"
#include "../Include/Assert.h"

CTimer::CTimer(CEventEngine *lpEngine, ITimerEvent *lpTimerEvent) : CEvent(lpEngine)
{
	Assert(lpTimerEvent != NULL);

	m_nTimerFD = -1;
	m_lpTimerEvent = lpTimerEvent;
}

CTimer::~CTimer()
{
	KillTimer();
}

Bool CTimer::SetTimer(Int32 nInterval)
{
	m_nTimerFD = timerfd_create(CLOCK_MONOTONIC, O_NONBLOCK);
	if (-1 == m_nTimerFD)
	{
		return False;
	}

	// itimerspec::it_value 的两个成员值同时设置为 0 时表示解除定时器
	//   如果想启用定时器时立即触发，可以将这两个值分别设置为 0 1
	itimerspec ts;
	ts.it_interval.tv_sec = nInterval / 1000;
	ts.it_interval.tv_nsec = (nInterval % 1000) * 1000000LL;
	ts.it_value.tv_sec = nInterval / 1000;
	ts.it_value.tv_nsec = (nInterval % 1000) * 1000000LL;
	
	if (timerfd_settime(m_nTimerFD, 0, &ts, NULL) != 0)
	{
		close(m_nTimerFD);
		m_nTimerFD = -1;
		return False;
	}

	if (!RegisterRead())
	{
		close(m_nTimerFD);
		m_nTimerFD = -1;
		return False;
	}

	return True;
}

void CTimer::KillTimer()
{
	if (m_nTimerFD != -1)
	{
		Unregister();
		close(m_nTimerFD);
		m_nTimerFD = -1;
	}
}

Int32 CTimer::GetFD() const
{
	return m_nTimerFD;
}

void CTimer::OnRead()
{
	Int64 lExpireTimes;
	read(m_nTimerFD, &lExpireTimes, sizeof(lExpireTimes));

	m_lpTimerEvent->OnTimer(this);
}

void CTimer::OnWrite()
{
	Assert(False);
}

void CTimer::OnError(Int32 nErrCode)
{
}
