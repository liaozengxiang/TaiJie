#include "../Include/MySQL.h"

CRecordset::CRecordset(MYSQL_RES *pResult)
{
    Assert(pResult != NULL);
    m_pResult = pResult;
    m_row = NULL;

    m_lRecordCount = mysql_num_rows(pResult);
}

CRecordset::~CRecordset()
{
    mysql_free_result(m_pResult);
    m_pResult = NULL;
}

Int64 CRecordset::GetRecordCount() const
{
    return m_lRecordCount;
}

Bool CRecordset::MoveNext()
{
    m_row = mysql_fetch_row(m_pResult);
    return (m_row != NULL);
}

LPCSTR CRecordset::GetFieldValue(Int32 nIndex) const
{
    return m_row[nIndex];
}


CMySQL::CMySQL()
{
    m_pMySQL = NULL;
}

CMySQL::~CMySQL()
{
    Close();
}

Bool CMySQL::Open(LPCSTR lpszHost, UInt16 uPort, LPCSTR lpszUserName, LPCSTR lpszPassword, LPCSTR lpszDatabase, LPCSTR lpszCharset)
{
    Assert(m_pMySQL == NULL);
    m_pMySQL = mysql_init(NULL);

    if (uPort == 0)
    {
        uPort = 3306;
    }

    if (NULL == lpszCharset)
    {
        lpszCharset = "utf8";
    }

    if (m_pMySQL != mysql_real_connect(m_pMySQL, lpszHost, lpszUserName, lpszPassword, lpszDatabase, uPort, 0, 0))
    {
        return False;
    }

    // 设置字符集
    char szCharset[64];
    Int32 nLen = sprintf(szCharset, "SET NAMES %s", lpszCharset);
    mysql_real_query(m_pMySQL, szCharset, nLen);

    return True;
}

void CMySQL::Close()
{
    if (m_pMySQL != NULL)
    {
        mysql_close(m_pMySQL);
        m_pMySQL = NULL;
    }
}

CRecordsetPtr CMySQL::Query(LPCSTR lpszSQL)
{
    if (mysql_real_query(m_pMySQL, lpszSQL, strlen(lpszSQL)))
    {
        return CRecordsetPtr(NULL);
    }

    MYSQL_RES *result = mysql_store_result(m_pMySQL);
    return CRecordsetPtr(new CRecordset(result));
}

Int32 CMySQL::Execute(LPCSTR lpszSQL)
{
    if (mysql_real_query(m_pMySQL, lpszSQL, strlen(lpszSQL)))
    {
        return -1;
    }

    return mysql_affected_rows(m_pMySQL);
}

UInt64 CMySQL::GetInsertID() const
{
    return mysql_insert_id(m_pMySQL);
}

UInt32 CMySQL::GetError() const
{
    return mysql_errno(m_pMySQL);
}

LPCSTR CMySQL::GetErrorDesc() const
{
    return mysql_error(m_pMySQL);
}
