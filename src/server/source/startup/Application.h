#ifndef APPLICATION__20171203
#define APPLICATION__20171203

#include <Common/Common.h>

class CApplication : protected CEventEngine, public IListenerEvent
{
public:
    CApplication();
    virtual ~CApplication();

public:
    Bool Start();

protected:
    virtual Bool OnAccept(Int32 nListenFD, Int32 nClientFD, const CSocketAddr &addr);
};

#endif
