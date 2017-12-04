/***********************************************************************
* Author:      ruanyanfang
* Date:        2017-03-21
* Description: ∂®“ÂCRedisString¿‡
***********************************************************************/
#include "Redis.h"

class CRedisCommand
{
public:
	CRedisCommand();
	~CRedisCommand();

public:
	ERWFlag GetRWFlag();

private:
	ERWFlag    m_eFlag;
};

class CRedisString
{
public:
	CRedisString(CRedis* pRedis);
	~CRedisString();

public:
	Bool   Set(const string& key, const string& value);
	string Get(const string& key);

private:
	CRedis*       m_pRedis;
};