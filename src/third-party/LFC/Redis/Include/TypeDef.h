/***********************************************************************
* Author:      ruanyanfang
* Date:        2017-03-24
* Description: 定义各种所需数据类型
***********************************************************************/
#ifndef __TYPEDEF_20170324__
#define __TYPEDEF_20170324__

#include <Common/Common.h>
using namespace std;

enum ReplyType
{
	REPLY_NONE,
	REPLY_STRING,
	REPLY_INTEGER,
	REPLY_BOOLEN,
	REPLY_LIST,
	REPLY_HASH,
	REPLY_ERROR
};

enum KeyType
{
	TYPE_NONE = 0,
	TYPE_STRING = 1,
	TYPE_LIST = 2,
	TYPE_SET = 3,
	TYPE_HASH = 4
};

enum ERWFlag
{
	READ,
	WRITE
};

typedef struct REDIS_INFO
{
	CSocketAddr     redisAddr;
	Int32           nConnectNum;
	struct timeval  tv;

	REDIS_INFO()
	{
		redisAddr.SetAddress("0.0.0.0:0");
		nConnectNum = 0;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
	}
}SRedisInfo;

typedef struct STRING_TYPE
{
	string strKey;
	string strValue;

	STRING_TYPE()
	{
		strKey = "";
		strValue = "";
	}
}SString;

typedef struct HASH_TYPE
{
	string              strKey;
	map<string, string> mapData;

	HASH_TYPE()
	{
		strKey = "";
		mapData.clear();
	}
}SHash;

typedef struct SET_TYPE
{
	string         strKey;
	vector<string> vecData;

	SET_TYPE()
	{
		strKey = "";
		vecData.clear();
	}
}SSet;

typedef struct List_TYPE
{
	string         strKey;
	list<string>   lstData;

	List_TYPE()
	{
		strKey = "";
		lstData.clear();
	}
}SList;

#endif
