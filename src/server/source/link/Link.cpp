#include <assert.h>
#include "Link.h"

CLink::CLink(Int32 nFD, CEventEngine *pEngine)
    : CEvent(pEngine), m_nFD(nFD)
{
}

CLink::~CLink()
{
    assert(m_nFD == -1);
}

Bool CLink::Initialize()
{
    return RegisterRead();
}

Int32 CLink::GetFD() const
{
    return m_nFD;
}

void CLink::OnRead()
{

}

void CLink::OnWrite()
{

}

void CLink::OnError(Int32 nErrCode)
{
    Close();
}

void CLink::Close()
{
    if (m_nFD != -1)
    {
        Unregister();

        close(m_nFD);
        m_nFD = -1;
    }
}
