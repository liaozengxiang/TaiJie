/***********************************************************************
* Author:      ruanyanfang
* Date:        2017-03-21
* Description: ÊµÏÖCRedisStringÀà
***********************************************************************/
#include "../Include/RedisString.h"

CRedisString::CRedisString(CRedis* pRedis)
{
	m_pRedis = pRedis;
}

CRedisString::~CRedisString()
{
}

Bool CRedisString::Set(const string & key, const string & value)
{
	Bool bSuccess = False;

	do 
	{
		string strCmd = "SET " + key + " " + value;
		CRedisReply cResult;
		m_pRedis->Excute(strCmd, cResult, WRITE);

	} while (0);

	return bSuccess;
}

string CRedisString::Get(const string & key)
{
	return string();
}

CRedisCommand::CRedisCommand()
{
}

CRedisCommand::~CRedisCommand()
{
}

ERWFlag CRedisCommand::GetRWFlag()
{
	return m_eFlag;
}

