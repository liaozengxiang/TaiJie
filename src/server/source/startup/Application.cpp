#include "Application.h"
#include "../link/Link.h"

CApplication::CApplication()
{
}

CApplication::~CApplication()
{
}

Bool CApplication::Start()
{
    Bool bSuccess = False;

    do
    {
        if (!Initialize())
        {
            break;
        }

        CInetListener listener(this, this);
        if (!listener.Start("0.0.0.0:2000"))
        {
            break;
        }

        while (1)
        {
            EventTest(500);
        }

        listener.Stop();
        bSuccess = True;
    } while (0);

    return bSuccess;
}

Bool CApplication::OnAccept(Int32 nListenFD, Int32 nClientFD, const CSocketAddr &addr)
{
    CLink *pLink = new CLink(nClientFD, this);
    if (!pLink->Initialize())
    {
        delete pLink;
        return False;
    }
    
    return True;
}
