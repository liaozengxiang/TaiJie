/*
 * CRecordsetPtr 使用示例: 
 *
 *     extern CRecordsetPtr res;
 *     while (res->MoveNext())
 *     {
 *         LPCSTR lpszName = res->GetFieldValue(0);
 *         LPCSTR lpszAge  = atoi(res->GetFieldValue(1));
 *     }
 */

#ifndef ___MYSQL__20160825___
#define ___MYSQL__20160825___

#include <Common/Common.h>
#include <mysql/mysql.h>

class CRecordset
{
public:
    CRecordset(MYSQL_RES *pResult);
    ~CRecordset();

protected:
    // 禁用拷贝构造函数与赋值函数
    CRecordset(const CRecordset &rhs);
    CRecordset& operator =(const CRecordset &rhs);

public:
    Int64  GetRecordCount() const;
    Bool   MoveNext();
    LPCSTR GetFieldValue(Int32 nIndex) const;

protected:
    MYSQL_RES*  m_pResult;
    MYSQL_ROW   m_row;
    Int64       m_lRecordCount;
};


/* 定义数据库记录集智能指针类型，用于查询时返回，对象资源自动释放 */
typedef CSharedPtr<CRecordset> CRecordsetPtr;

class CMySQL
{
public:
    CMySQL();
    ~CMySQL();

public:
    Bool Open(LPCSTR lpszHost, UInt16 uPort, LPCSTR lpszUserName, LPCSTR lpszPassword, LPCSTR lpszDatabase, LPCSTR lpszCharset = "utf8");
    void Close();

    CRecordsetPtr Query(LPCSTR lpszSQL);
    Int32         Execute(LPCSTR lpszSQL);
    UInt64        GetInsertID() const;

    // 当 Query/Execute 返回失败时获取错误信息
    UInt32 GetError() const;
    LPCSTR GetErrorDesc() const;

protected:
    // 禁用拷贝构造函数与赋值函数
    CMySQL(const CMySQL &rhs);
    CMySQL& operator =(const CMySQL &rhs);

protected:
    MYSQL*  m_pMySQL;
};

#endif
