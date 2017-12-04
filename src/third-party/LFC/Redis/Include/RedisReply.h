/***********************************************************************
* Author:      ruanyanfang
* Date:        2017-03-24
* Description: 定义CRedisReply类，存放执行redis命令后的结果
***********************************************************************/
#ifndef __REDIS_REPLY_20170324__
#define __REDIS_REPLY_20170324__

#include <hiredis/hiredis.h>
#include <Common/Common.h>
#include "TypeDef.h"
using namespace std;

class CRedisReply
{
public:
	CRedisReply();
	~CRedisReply();

public:	
	ReplyType           GetReplyType();

	string			    GetReplyErr();
	Bool				GetReplyBool();
	string			    GetReplyString();
	UInt64				GetReplyInteger();
	list<string>        GetReplyList();
	map<string, string> GetReplyHash();

private:
	friend class CRedis;
	void SetReplyType(ReplyType type);
	void Clear();

	void SetReplyErr(const string& strErrReply);
	void SetReplyBool(const Bool& bBoolReply);
	void SetReplyString(const string& strStringReply);
	void SetReplyInteger(const Int64 lIntegerReply);
	void SetReplyList(redisReply* pReply);
	void SetReplyHash(redisReply* pReply);

private:
	ReplyType			     m_eReplyType;

	string					 m_strReplyErr;
	Bool				     m_boolReplyBool;
	string					 m_strReplyString;
	UInt64					 m_lReplyInterger;
	list<string>			 m_lstReplyList;
	map<string, string>      m_mapReplyHash;
};

#endif
