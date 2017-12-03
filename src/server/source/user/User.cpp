#include "User.h"

CUser::CUser(LPCSTR lpszUserID, LPCSTR lpszNickname)
{
    m_strUserID = lpszUserID;
    m_strNickname = lpszNickname;
}

CUser::~CUser()
{
}
