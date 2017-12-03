#ifndef LINK__20171203
#define LINK__20171203

#include <Common/Common.h>

class CLink : public CEvent
{
public:
    CLink(Int32 nFD, CEventEngine *pEngine);
    virtual ~CLink();

public:
    Bool Initialize();

protected:
    virtual Int32 GetFD() const;
    virtual void OnRead();
    virtual void OnWrite();
    virtual void OnError(Int32 nErrCode);

protected:
    void Close();

protected:
    Int32           m_nFD;
};

#endif
