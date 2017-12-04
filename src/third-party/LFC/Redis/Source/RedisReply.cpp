/***********************************************************************
* Author:      ruanyanfang
* Date:        2017-03-24
* Description:  µœ÷CRedisReply¿‡
***********************************************************************/
#include "../Include/RedisReply.h"

CRedisReply::CRedisReply()
{
	m_eReplyType = REPLY_NONE;
	m_strReplyErr = "";
	m_boolReplyBool = False;
	m_strReplyString = "";
	m_lReplyInterger = 0;
}

CRedisReply::~CRedisReply()
{
}

ReplyType CRedisReply::GetReplyType()
{
	return m_eReplyType;
}

string CRedisReply::GetReplyErr()
{
	return m_strReplyErr;
}

Bool CRedisReply::GetReplyBool()
{
	return m_boolReplyBool;
}

string CRedisReply::GetReplyString()
{
	return m_strReplyString;
}

UInt64 CRedisReply::GetReplyInteger()
{
	return m_lReplyInterger;
}

list<string> CRedisReply::GetReplyList()
{
	return m_lstReplyList;
}

map<string, string> CRedisReply::GetReplyHash()
{
	return m_mapReplyHash;
}

void CRedisReply::SetReplyType(ReplyType type)
{
	m_eReplyType = type;
}

void CRedisReply::Clear()
{
	m_eReplyType = REPLY_NONE;
}

void CRedisReply::SetReplyErr(const string & strErrReply)
{
	m_strReplyErr = strErrReply;
}

void CRedisReply::SetReplyBool(const Bool & bBoolReply)
{
	m_boolReplyBool = bBoolReply;
}

void CRedisReply::SetReplyString(const string & strStringReply)
{
	m_strReplyString = strStringReply;
}

void CRedisReply::SetReplyInteger(const Int64 lIntergerReply)
{
	m_lReplyInterger = lIntergerReply;
}

void CRedisReply::SetReplyList(redisReply* pReply)
{
	for (UInt32 i = 0; i < pReply->elements; i++)
	{
		if (pReply->element[i]->type == REDIS_REPLY_STRING)
		{
			m_lstReplyList.push_back(pReply->element[i]->str);
		}
		else if (pReply->element[i]->type == REDIS_REPLY_NIL)
		{
			m_lstReplyList.push_back("");
		}
	}


	//m_lstReplyList = lstListReply;
}

void CRedisReply::SetReplyHash(redisReply* pReply)
{
	for (UInt32 i = 0; i < pReply->elements; i += 2)
	{
		string strField = pReply->element[i]->str;
		string strValue = pReply->element[i + 1]->str;
		m_mapReplyHash[strField] = strValue;
	}
}
