#ifndef USER__MGR__20171203
#define USER__MGR__20171203

#include <map>
#include "User.h"
using namespace std;

class CUserMgr
{
    DECLARE_SINGLETON(CUserMgr)

protected:
    CUserMgr();
    ~CUserMgr();

public:
    Bool Login(LPCSTR lpszLoginID, LPCSTR lpszPassword);

protected:
    map<Int64, CUser*>          m_mapUsers;         // user_id -> CUser*
    CMutex                      m_lock;
};

#endif
