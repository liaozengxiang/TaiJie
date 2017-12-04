/***********************************************************************
* Author:      ruanyanfang
* Date:        2017-03-20
* Description: 定义CRedis类
***********************************************************************/
#ifndef __REDIS_H__20170320__
#define __REDIS_H__20170320__

#include <hiredis/hiredis.h>
#include <Common/Common.h>
#include "TypeDef.h"
#include "RedisReply.h"
using namespace std;

class CRedis : public ITimerEvent
{
public:
	CRedis();
	~CRedis();

public:
	// ITimerEvent
	virtual void  OnTimer(CTimer *pTimer);

	Bool          Initialize(SRedisInfo& redisInfo, CEventEngine* pEngine);
	Int32         InitConnections();
	void          CloseConnections();

public:
	///////////////////////string 相关操作/////////////////////////////////////////
	Bool		  Set(const SString& data);
	Bool		  Get(SString& data);

	///////////////////////hash 相关操作/////////////////////////////////////////
	Bool		  Set(const SHash& data);
	Bool		  GetAll(SHash& data);
	Bool		  GetAllField(const string& strKey, list<string>& lstFields);
	Bool		  Get(const string& strHashKey, const list<string>& lstField, list<string>& lstValue);
	
	///////////////////////list 相关操作/////////////////////////////////////////
	Bool		  Set(const string& strListKey, const Int32 nIndex, const string& strValue);
	Bool		  GetAll(SList& data);
	Bool		  GetByRange(SList& data, const Int32 nStart, const Int32 nEnd);
	string		  GetByIndex(const string& strKey, Int32 nIndex);
	string		  Front(const string& strKey);
	string		  Back(const string& strKey);
	Bool		  PushBack(const SList& data);
	Bool		  PushFront(const SList& data);
	Bool          PopBack(const string& strKey);
	Bool          PopFront(const string& strKey);

	///////////////////////set 相关操作/////////////////////////////////////////
	Bool		  Set(const SSet& data);
	Bool		  GetAll(SSet& data);
	Bool          Del(const SSet& data);
	Bool          Move(const string& strSrcKey, const string& strDetKey, const string& strValue);

	///////////////////////key 相关操作/////////////////////////////////////////
	Bool		  IsExist(const string& strKey);
	Bool		  Del(const string& strKey);
	Bool		  Del(const list<string>& lstKeys);
	KeyType		  Type(const string& strKey);
	Bool		  SetExpire(const string& strKey, Int64 nSeconds);
	Bool		  SetExpireByUnixtime(const string& strKey, Int64 nMilliseconds);
	Int64		  TimeToExpire(const string& strKey);
	list<string>  KeysWithPattern(const string& strPattern);

	///////////////////////通用操作/////////////////////////////////////////////
	// 获取set、list的长度，获取hash的field数量
	Int32         Length(const string& strKey);
	// hash\set的key对应的field\value是否存在
	Bool          IsExist(const string& strKey, const string& strData);
	// 删除hash\set的key对应的field\value
	Bool          Del(const string& strKey, const string& strData);

private:
	// 执行组装好的命令，得到执行结果
	void                Excute(const string& strCmd, CRedisReply& result, ERWFlag flag);
	void                ExcuteRead(redisContext* pContext, int argc, const char **argv, const string& strCmd, CRedisReply& result);
	void                ExcuteWrite(redisContext* pContext, int argc, const char **argv, const string& strCmd, CRedisReply& result);

	// 从m_lstRedisConnect中获取一个可用的链接
	redisContext*       GetConnection();
	// 链接使用完后放回m_lstRedisConnect中
	void                ReleaseConnection(redisContext* pContext);

	// 将redisReply解析为CRedisReply，READ
	void                PraseReply2Result4Read(redisReply* pReply, CRedisReply& result, const string& strCmd);
	// 将redisReply解析为CRedisReply，WRITE
	void                PraseReply2Result4Write(redisReply* pReply, CRedisReply& result, const string& strCmd);
	// 将redisReply解析为map
	map<string, string> PraseReplyHash(redisReply* pReply);
	// 将redisReply解析为list
	//list<string>        PraseReplyList(redisReply* pReply);

	// 组装Excute所需参数
	void                GenerateParam(const string& strCmd, Int32 nArgc, const char **argv);

private:
	
	list<redisContext*>       m_lstRedisConnect;
	CMutex                    m_mtxRedisConnectList;

	CSocketAddr               m_addrRedis;
	Int32                     m_nMaxConnSum;
	struct  timeval           m_tv;

	CTimer*                   m_pTimer;
	CEventEngine*             m_pEngine;
};

#endif