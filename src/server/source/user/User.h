#ifndef USER__20171203
#define USER__20171203

#include <Common/Common.h>
#include <string>
using namespace std;

class CUser
{
public:
    CUser(LPCSTR lpszUserID, LPCSTR lpszNickname);
    ~CUser();

public:
    LPCSTR GetUserID() const { return m_strUserID.c_str(); }
    LPCSTR GetNickname() const { return m_strNickname.c_str(); }

protected:
    string              m_strUserID;
    string              m_strNickname;
};

#endif
