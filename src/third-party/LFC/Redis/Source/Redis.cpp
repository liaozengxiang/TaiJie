/***********************************************************************
 * Author:      ruanyanfang
 * Date:        2017-03-20
 * Description: 实现redis.h中定义的函数
***********************************************************************/
#include <stdio.h>
#include <time.h>
#include <sstream>
#include "../Include/Redis.h"

CRedis::CRedis()
{

}

CRedis::~CRedis()
{
	CloseConnections();

	m_pTimer->KillTimer();
	delete m_pTimer;
	m_pTimer = NULL;
}

void CRedis::OnTimer(CTimer * pTimer)
{
	m_mtxRedisConnectList.Lock();

	list<redisContext*>::iterator it = m_lstRedisConnect.begin();
	while (it != m_lstRedisConnect.end())
	{
		redisContext* pTmp = *it;
		if (pTmp->err)
		{
			redisFree(pTmp);
			m_lstRedisConnect.erase(it++);
		}
		else
		{
			++it;
		}
	}

	Int32 nSize = m_nMaxConnSum - m_lstRedisConnect.size();
	for (Int32 i = 0; i < nSize; ++i)
	{
		redisContext* pTmp = redisConnectWithTimeout(m_addrRedis.GetIPString().c_str(), m_addrRedis.GetPort(), m_tv);
		if (pTmp != NULL)
		{
			m_lstRedisConnect.push_back(pTmp);
		}
	}

	m_mtxRedisConnectList.Unlock();
}

Bool CRedis::Initialize(SRedisInfo& redisInfo, CEventEngine* pEngine)
{
	m_pEngine = pEngine;
	m_pTimer = new CTimer(pEngine, this);

	m_addrRedis = redisInfo.redisAddr;
	m_nMaxConnSum = redisInfo.nConnectNum;
	m_tv.tv_sec = redisInfo.tv.tv_sec;
	m_tv.tv_usec = redisInfo.tv.tv_usec;

	if (InitConnections() != m_nMaxConnSum)
	{
		return False;
	}

	if (!m_pTimer->SetTimer(500))// 
	{
		return False;
	}

	return True;
}

Int32 CRedis::InitConnections()
{	
	do
	{
		for (Int32 i = 0; i < m_nMaxConnSum; i++)
		{
			redisContext* pTmpContext = redisConnectWithTimeout(m_addrRedis.GetIPString().c_str(), m_addrRedis.GetPort(), m_tv);
			if (pTmpContext->err)
			{
				//ERROR_LOG("InitConnections(): connect to redis server--" << m_addrRedis.GetAddrString().c_str() << " failed, err is: " << pTmpContext->errstr);
				
				if (pTmpContext)
				{
					redisFree(pTmpContext);
					pTmpContext = NULL;
				}
				continue;
			}
			else
			{
				//INFO_LOG("InitConnections(): connected to redis server, fd is: " << pTmpContext->fd);

				m_mtxRedisConnectList.Lock();
				m_lstRedisConnect.push_back(pTmpContext);
				m_mtxRedisConnectList.Unlock();
			}
		}
	} while (0);

	return m_lstRedisConnect.size();
}

void CRedis::CloseConnections()
{
	list<redisContext*>::iterator it = m_lstRedisConnect.begin();
	m_mtxRedisConnectList.Lock();

	for (; it != m_lstRedisConnect.end(); it++)
	{
		redisContext* pTmp = *it;
		if (pTmp)
		{
			//INFO_LOG("CloseConnections(): redis connect to be freed, fd is: " << pTmp->fd);

			redisFree(pTmp);
			pTmp = NULL;
		}
	}
	m_lstRedisConnect.clear();

	m_mtxRedisConnectList.Unlock();
}

/*************************************************
Function:       CRedis::Set()
Description:    string的set操作
Command:		SET key value
Calls:          
Table Accessed: 
Table Updated: 
Input:          SString结构体，包括key和value
Output:         在设置操作成功完成时，才返回True
Return:         
Others:         
*************************************************/
Bool CRedis::Set(const SString & data)
{
	string strCmd = "SET " + data.strKey + " " + data.strValue;
	CRedisReply cResult;
	Excute(strCmd, cResult, WRITE);

	return (cResult.GetReplyType() == REPLY_ERROR) ? False : True;
}

/*************************************************
Function:       CRedis::Set()
Description:    list的set操作
Command:		LSET key index value
Calls:
Table Accessed:
Table Updated:
Input:
Output:         索引超出范围或对空列表进行操作时返回False
				设置成功返回True
Return:
Others:
*************************************************/
Bool CRedis::Set(const string& strListKey, const Int32 nIndex, const string& strValue)
{
	CRedisReply cResult;
	string strCmd = "LSET " + strListKey + " " + STR::NumberToString(nIndex) + " " + strValue;
	Excute(strCmd, cResult, WRITE);

	return (cResult.GetReplyType() == REPLY_ERROR) ? False : True;
}

/*************************************************
Function:       CRedis::Get()
Description:    list的get操作
Command:		LRANGE key 0 -1
Calls:
Table Accessed:
Table Updated:
Input:			只需SList的strKey设值
Output:         key为list时返回True，否则返回False
				key不存在时lstData为空
Return:
Others:
*************************************************/
Bool CRedis::GetAll(SList & data)
{
	return GetByRange(data, 0, -1);
}

/*************************************************
Function:       CRedis::Get()
Description:    list的get操作
Command:		LRANGE key start end
Calls:
Table Accessed:
Table Updated:
Input:			只需SList的strKey设值
Output:         key为list时返回True，否则返回False
				key不存在时lstData为空
				nStart与nEnd非法时lstData为空
Return:
Others:
*************************************************/
Bool CRedis::GetByRange(SList & data, const Int32 nStart, const Int32 nEnd)
{
	Bool bSuccess = False;
	CRedisReply cResult;
	data.lstData.clear();

	do
	{
		string strCmd = "LRANGE " + data.strKey + " " + STR::NumberToString(nStart) + " " + STR::NumberToString(nEnd);
		Excute(strCmd, cResult, READ);

		if (cResult.GetReplyType() == REPLY_ERROR)
		{
			break;
		}
		else
		{
			data.lstData = cResult.GetReplyList();
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Get()
Description:    set的get操作
Command:		SMEMBERS key
Calls:
Table Accessed:
Table Updated:
Input:			只需SSet的strKey设值
Output:         key为set时返回True，否则返回False
				key不存在时vecData为空
Return:
Others:
*************************************************/
Bool CRedis::GetAll(SSet& data)
{
	Bool bSuccess = False;
	CRedisReply cResult;
	data.vecData.clear();
	
	do
	{
		string strCmd = "SMEMBERS " + data.strKey;
		
		Excute(strCmd, cResult, READ);

		if (cResult.GetReplyType() == REPLY_ERROR)
		{
			break;
		}
		else
		{
			/*list<string> lstTmp;
			lstTmp = cResult.GetReplyList();
			list<string>::const_iterator it = lstTmp.begin();
			for (; it != lstTmp.end(); it++)
			{
				data.vecData.push_back(*it);
			}*/
			list<string>::const_iterator it = cResult.GetReplyList().begin();
			for (; it != cResult.GetReplyList().end(); it++)
			{
				data.vecData.push_back(*it);
			}
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Del()
Description:    set的删除多个value操作
Command:		SREM key value1...
Calls:
Table Accessed:
Table Updated:
Input:			
Output:         key不是set类型，返回False
				待删除的元素都不存在时，返回False
				待删除元素部分存在且操作成功时，返回True，不存在元素会被忽略
Return:
Others:
*************************************************/
Bool CRedis::Del(const SSet & data)
{
	Bool bSuccess = False;
	CRedisReply cResult;
	string strCmd, strTmp;

	do 
	{
		UInt32 i = 0;
		vector<string>::const_iterator it = data.vecData.begin();
		for (; it != data.vecData.end(); it++, i++)
		{
			if (i < data.vecData.size() - 1)
			{
				strTmp += (*it) + " ";
			}
			else
			{
				strTmp += (*it);
			}
		}
		strCmd = "SREM " + data.strKey + " " + strTmp;

		Excute(strCmd, cResult, WRITE);

		if (cResult.GetReplyType() == REPLY_ERROR || cResult.GetReplyInteger() == 0)
		{
			break;
		}
		else
		{
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Move()
Description:    将srcKey中的value元素移到detKey中
Command:		SMOVE srcKey detKey value
Calls:
Table Accessed:
Table Updated:
Input:
Output:         srcKey或detKey不是set类型，返回False
				value在srcKey中不存在，返回False
				value在srcKey或detKey中存在，返回True
Return:
Others:
*************************************************/
Bool CRedis::Move(const string & strSrcKey, const string & strDetKey, const string & strValue)
{
	Bool bSuccess = False;
	CRedisReply cResult;
	string strCmd;

	do 
	{
		strCmd = "SMOVE " + strSrcKey + " " + strDetKey + " " + strValue;

		Excute(strCmd, cResult, WRITE);

		if (cResult.GetReplyType() == REPLY_ERROR || cResult.GetReplyInteger() == 0)
		{
			break;
		}
		else
		{
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::GetByIndex()
Description:    list的get操作
Command:		LINDEX key index
Calls:
Table Accessed:
Table Updated:
Input:			strKey必须为list类型的key
Output:         返回实际值或""
Return:
Others:
*************************************************/
std::string CRedis::GetByIndex(const string& strKey, Int32 nIndex)
{
	CRedisReply cResult;
	string strCmd = "LINDEX " + strKey + " " + STR::NumberToString(nIndex);
	Excute(strCmd, cResult, READ);

	return cResult.GetReplyString();
}

/*************************************************
Function:       CRedis::GetByIndex()
Description:    list的get操作，获取list的第1个元素
Command:		LINDEX key 0
Calls:
Table Accessed:
Table Updated:
Input:			strKey必须为list类型的key
Output:         返回实际值或""
Return:
Others:
*************************************************/
std::string CRedis::Front(const string& strKey)
{
	return GetByIndex(strKey, 0);
}

/*************************************************
Function:       CRedis::GetByIndex()
Description:    list的get操作，获取list的最后1个元素
Command:		LINDEX key -1
Calls:
Table Accessed:
Table Updated:
Input:			strKey必须为list类型的key
Output:         返回实际值或""
Return:
Others:
*************************************************/
std::string CRedis::Back(const string& strKey)
{
	return GetByIndex(strKey, -1);
}

Bool CRedis::Set(const SSet & data)
{
	CRedisReply cResult;
	string strCmd, strTmp;

	UInt32 i = 0;
	vector<string>::const_iterator it = data.vecData.begin();
	for (; it != data.vecData.end(); it++, i++)
	{
		if (i < data.vecData.size() - 1)
		{
			strTmp += *it + " ";
		}
		else
		{
			strTmp += *it;
		}
		strCmd = "SADD " + data.strKey + " " + strTmp;
	}
	
	Excute(strCmd, cResult, WRITE);

	return (cResult.GetReplyType() == REPLY_ERROR) ? False : True;
}

/*************************************************
Function:       CRedis::IsExist()
Description:    key是否存在
Command:		EXISTS key 
Calls:
Table Accessed:
Table Updated:
Input:
Output:         key存在时返回True
Return:
Others:
*************************************************/
Bool CRedis::IsExist(const string& strKey)
{
	Bool bSuccess = False;
	CRedisReply cResult;

	do 
	{
		string strCmd = "EXISTS " + strKey;
		Excute(strCmd, cResult, READ);

		if (cResult.GetReplyType() == REPLY_BOOLEN && cResult.GetReplyBool() == True)
		{
			bSuccess = True;
		}
		else
		{
			break;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Del()
Description:    删除1个key
Command:		DEL key
Calls:
Table Accessed:
Table Updated:
Input:
Output:         成功删除时返回True
				key不存在或执行失败时返回False
Return:
Others:
*************************************************/
Bool CRedis::Del(const string& strKey)
{
	Bool bSuccess = False;
	CRedisReply cResult;

	do 
	{
		string strCmd = "DEL " + strKey;
		Excute(strCmd, cResult, WRITE);

		if (cResult.GetReplyType() == REPLY_INTEGER && cResult.GetReplyInteger() == 1)
		{
			bSuccess = True;
		}
		else
		{
			break;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Del()
Description:    删除多个key
Command:		DEL key1...
Calls:
Table Accessed:
Table Updated:
Input:
Output:         全部成功删除时返回True
				部分key不存在或执行失败时返回False
Return:
Others:
*************************************************/
Bool CRedis::Del(const list<string>& lstKeys)
{
	Bool bSuccess = False;
	string strTmp, strCmd;
	CRedisReply cResult;

	do
	{
		UInt32 i = 0;
		list<string>::const_iterator it = lstKeys.begin();
		for (; it != lstKeys.end(); it++, i++)
		{
			if (i < lstKeys.size() - 1)
			{
				strTmp += (*it) + " ";
			}
			else
			{
				strTmp += (*it);
			}
		}
		strCmd = "DEL " + strTmp;
				
		Excute(strCmd, cResult, WRITE);

		if (cResult.GetReplyType() == REPLY_INTEGER && cResult.GetReplyInteger() == lstKeys.size())
		{
			bSuccess = True;
		}
		else
		{
			break;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Type()
Description:    获取key的类型
Command:		TYPE key
Calls:
Table Accessed:
Table Updated:
Input:
Output:         返回key的类型
Return:
Others:
*************************************************/
KeyType CRedis::Type(const string& strKey)
{
	KeyType res;
	CRedisReply cResult;
	string strCmd = "TYPE " + strKey;
	
	Excute(strCmd, cResult, READ);
	string strTmp = cResult.GetReplyString();

	if (strTmp == "string")
	{
		res = TYPE_STRING;
	}
	else if (strTmp == "list")
	{
		res = TYPE_LIST;
	}
	else if (strTmp == "set")
	{
		res = TYPE_SET;
	}
	else if (strTmp == "hash")
	{
		res = TYPE_HASH;
	}
	else
	{
		res = TYPE_NONE;
	}

	return res;
}

/*************************************************
Function:       CRedis::SetExpire()
Description:    设置key的过期时间，seconds秒后该key不可用
Command:		EXPIRE key seconds
Calls:
Table Accessed:
Table Updated:
Input:			
Output:         key存在且设置成功时返回True
				key不存在或执行失败时返回Flase
Return:
Others:
*************************************************/
Bool CRedis::SetExpire(const string& strKey, Int64 nSeconds)
{
	Bool bSuccess = False;
	CRedisReply cResult;

	do
	{
		string strCmd = "EXPIRE " + strKey + " " + STR::NumberToString(nSeconds);
		Excute(strCmd, cResult, WRITE);
		if (cResult.GetReplyType() == REPLY_INTEGER && cResult.GetReplyInteger() == 1)
		{
			bSuccess = True;
		}
		else
		{
			break;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::SetExpire()
Description:    设置key的过期时间，时间为unix时间戳
Command:		EXPIREAT key unix_timestamp
Calls:
Table Accessed:
Table Updated:
Input:
Output:         key存在且设置成功时返回True
				key不存在或执行失败时返回Flase
Return:
Others:
*************************************************/
Bool CRedis::SetExpireByUnixtime(const string& strKey, Int64 nMilliseconds)
{
	Bool bSuccess = False;
	CRedisReply cResult;

	do
	{
		string strCmd = "EXPIREAT " + strKey + " " + STR::NumberToString(nMilliseconds);
		Excute(strCmd, cResult, WRITE);

		if (cResult.GetReplyType() == REPLY_INTEGER && cResult.GetReplyInteger() == 1)
		{
			bSuccess = True;
		}
		else
		{
			break;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::TimeToExpire()
Description:    返回key的剩余过期时间，单位为秒
Command:		TTL key
Calls:
Table Accessed:
Table Updated:
Input:
Output:         key存在且已经设置过期时间，返回剩余时间
				key存在且未设置过期时间，返回-1
				key不存在返回-2
				执行失败返回-3
Return:
Others:
*************************************************/
Int64 CRedis::TimeToExpire(const string& strKey)
{
	CRedisReply cResult;
	string strCmd = "TTL " + strKey;
	
	Excute(strCmd, cResult, READ);
	if (cResult.GetReplyType() == REPLY_ERROR)
	{
		return -3;
	}
	else
	{
		return cResult.GetReplyInteger();
	}
}

std::list<std::string> CRedis::KeysWithPattern(const string& strPattern)
{
	CRedisReply cResult;
	string strCmd = "KEYS " + strPattern;
	Excute(strCmd, cResult, READ);

	return cResult.GetReplyList();
}


/*************************************************
Function:       CRedis::Length()
Description:    获取list、set的长度，获取hash的field数量
Command:		LLEN key  
				SCARD key 
				HLEN key
Calls:
Table Accessed:
Table Updated:
Input:
Output:         -2，表示执行失败
				-1，表示key为string，不支持查询长度
				0， 表示key不存在
				key类型为list、set、hash且存在，返回长度
Return:
Others:
*************************************************/
Int32 CRedis::Length(const string& strKey)
{
	CRedisReply cResult;
	string strCmd;
	KeyType type = Type(strKey);
	switch (type)
	{
	case TYPE_NONE:
		return 0;
		break;
	case TYPE_STRING:
		return -1;
		break;
	case TYPE_LIST:
		strCmd = "LLEN " + strKey;
		break;
	case TYPE_SET:
		strCmd = "SCARD " + strKey;
		break;
	case TYPE_HASH:
		strCmd = "HLEN " + strKey;
		break;
	default:
		return 0;
		break;
	}
	
	Excute(strCmd, cResult, READ);
	return (cResult.GetReplyType() == REPLY_ERROR) ? -2: (cResult.GetReplyInteger());
}

Bool CRedis::IsExist(const string & strKey, const string & strData)
{
	string strCmd;
	CRedisReply cResult;

	KeyType type = Type(strKey);
	switch (type)
	{
	case TYPE_NONE:
		return False;
		break;
	case TYPE_SET:
		strCmd = "SISMEMBER " + strKey + " " + strData;
		break;
	case TYPE_HASH:
		strCmd = "HEXISTS " + strKey + " " + strData;
		break;
	default:
		return False;
		break;
	}
	
	Excute(strCmd, cResult, READ);
	if (cResult.GetReplyType() == REPLY_ERROR || cResult.GetReplyBool() == False)
	{
		return False;
	}
	else
	{
		return True;
	}
}

Bool CRedis::Del(const string & strKey, const string & strData)
{
	string strCmd;

	KeyType type = Type(strKey);
	switch (type)
	{
	case TYPE_NONE:
		return False;
		break;
	case TYPE_SET:
		strCmd = "SREM " + strKey + " " + strData;
		break;
	case TYPE_HASH:
		strCmd = "HDEL " + strKey + " " + strData;
		break;
	default:
		return False;
		break;
	}

	CRedisReply cResult;
	Excute(strCmd, cResult, WRITE);
		
	if (cResult.GetReplyType() == REPLY_ERROR || cResult.GetReplyInteger() < 0)
	{
		return False;
	}
	else
	{
		return True;
	}
}

/*************************************************
Function:       CRedis::PushBack()
Description:    list的插入操作
Command:		RPUSH key value
				RPUSH key value1...
Calls:
Table Accessed:
Table Updated:
Input:
Output:         在设置操作成功完成时，返回True
Return:
Others:
*************************************************/
Bool CRedis::PushBack(const SList& data)
{
	string strCmd;
	CRedisReply cResult;

	string strTmp;
	UInt32 i = 0;
	list<string>::const_iterator it = data.lstData.begin();
	for (; it != data.lstData.end(); it++, i++)
	{
		if (i < data.lstData.size() - 1)
		{
			strTmp += *(it)+" ";
		}
		else
		{
			strTmp += *(it);
		}
	}
	strCmd = "RPUSH " + data.strKey + " " + strTmp;

	Excute(strCmd, cResult, WRITE);

	return (cResult.GetReplyType() == REPLY_ERROR) ? False : True;
}

/*************************************************
Function:       CRedis::PushFront()
Description:    list的插入操作
Command:		LPUSH key value
				LPUSH key value1...
Calls:
Table Accessed:
Table Updated:
Input:
Output:         在设置操作成功完成时，返回True
Return:
Others:
*************************************************/
Bool CRedis::PushFront(const SList& data)
{
	string strCmd;
	CRedisReply cResult;

	string strTmp;
	UInt32 i = 0;
	list<string>::const_iterator it = data.lstData.begin();
	for (; it != data.lstData.end(); it++, i++)
	{
		if (i < data.lstData.size() - 1)
		{
			strTmp += *(it)+" ";
		}
		else
		{
			strTmp += *(it);
		}
	}
	strCmd = "LPUSH " + data.strKey + " " + strTmp;
	
	Excute(strCmd, cResult, WRITE);

	return (cResult.GetReplyType() == REPLY_ERROR) ? False : True;
}

/*************************************************
Function:       CRedis::PopBack()
Description:    list的移除操作
Command:		RPOP key 
Calls:
Table Accessed:
Table Updated:
Input:
Output:         当key不为list类型时，返回False
				当key不存在时，返回False
				当key存在时，返回True
Return:
Others:
*************************************************/
Bool CRedis::PopBack(const string & strKey)
{
	Bool bSuccess = False;
	string strCmd;
	CRedisReply cResult;

	do
	{
		strCmd = "RPOP " + strKey;
		Excute(strCmd, cResult, WRITE);

		if (cResult.GetReplyType() == REPLY_ERROR || cResult.GetReplyString() == "")
		{
			break;
		}
		else
		{
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::PopBack()
Description:    list的移除操作
Command:		LPOP key
Calls:
Table Accessed:
Table Updated:
Input:
Output:         当key不为list类型时，返回False
				当key不存在时，返回False
				当key存在时，返回True
Return:
Others:
*************************************************/
Bool CRedis::PopFront(const string & strKey)
{
	Bool bSuccess = False;
	string strCmd;
	CRedisReply cResult;

	do
	{
		strCmd = "LPOP " + strKey;
		Excute(strCmd, cResult, WRITE);

		if (cResult.GetReplyType() == REPLY_ERROR || cResult.GetReplyString() == "")
		{
			break;
		}
		else
		{
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Set()
Description:    string的get操作
Command:		GET key
Calls:
Table Accessed:
Table Updated:
Input:          
Output:         key存在时返回value，key不存在时返回""
Return:
Others:
*************************************************/
Bool CRedis::Get(SString& data)
{
	Bool bSuccess = False;

	do 
	{
		string strCmd = "GET " + data.strKey;
		CRedisReply cResult;
		Excute(strCmd, cResult, READ);

		if (cResult.GetReplyType() == REPLY_ERROR)
		{
			break;
		}
		else
		{
			data.strValue = cResult.GetReplyString();
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Set()
Description:    hash的set操作
Command:		HSET key field value
				HMSET key field1 value1...
Calls:
Table Accessed:
Table Updated:
Input:          
Output:         在设置操作成功完成时，才返回True
Return:
Others:
*************************************************/
Bool CRedis::Set(const SHash & data)
{
	string strCmd;
	if (data.mapData.size() == 1)
	{
		strCmd = "HSET " + data.strKey + " " + data.mapData.begin()->first + " " + data.mapData.begin()->second;
	}
	else
	{
		string strTmp;
		UInt32 i = 0;
		map<string, string>::const_iterator it = data.mapData.begin();
		for (; it != data.mapData.end(); it++, i++)
		{
			if (i < data.mapData.size() - 1)
			{
				strTmp += it->first + " " + it->second + " ";
			}
			else
			{
				strTmp += it->first + " " + it->second;
			}
		}
		strCmd = "HMSET " + data.strKey + " " + strTmp;
	}

	CRedisReply cResult;
	Excute(strCmd, cResult, WRITE);

	return (cResult.GetReplyType() == REPLY_ERROR) ? False : True;
}

/*************************************************
Function:       CRedis::Get()
Description:    hash的get操作
Command:		HGETALL key
Calls:
Table Accessed:
Table Updated:
Input:			只需SHash的strKey设值
Output:         key为hash时返回True，否则返回False
				key不存在时mapData为空
Return:
Others:
*************************************************/
Bool CRedis::GetAll(SHash& data)
{
	Bool bSuccess = False;
	data.mapData.clear();

	do 
	{
		string strCmd = "HGETALL " + data.strKey;
		CRedisReply cResult;
		Excute(strCmd, cResult, READ);

		if (cResult.GetReplyType() == REPLY_ERROR)
		{
			break;
		}
		else
		{
			data.mapData = cResult.GetReplyHash();
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Get()
Description:    hash的get操作
Command:		HKEYS key
Calls:
Table Accessed:
Table Updated:
Input:			
Output:         key为hash时返回True，否则返回False
				key不存在时lstFields为空
Return:
Others:
*************************************************/
Bool CRedis::GetAllField(const string & strKey, list<string>& lstFields)
{
	Bool bSuccess = False;
	lstFields.clear();

	do 
	{
		string strCmd = "HKEYS " + strKey;
		CRedisReply cResult;
		Excute(strCmd, cResult, READ);

		if (cResult.GetReplyType() == REPLY_ERROR)
		{
			break;
		}
		else
		{
			lstFields = cResult.GetReplyList();
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

/*************************************************
Function:       CRedis::Get()
Description:    hash的get操作
Command:		HMGET key field1...
Calls:
Table Accessed:
Table Updated:
Input:
Output:         key不为hash时返回False
				key不存在时lstValue中的元素全为""
				指定field不存在时对应value为""
				lstValue与lstField一一对应
Return:
Others:
*************************************************/
Bool CRedis::Get(const string & strHashKey, const list<string>& lstField, list<string>& lstValue)
{
	Bool bSuccess = False;
	lstValue.clear();
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
	do 
	{
		string strTmp, strCmd;
		CRedisReply cResult;

		list<string>::const_iterator it = lstField.begin();
		UInt32 i = 0;
		for (; it != lstField.end(); it++, i++)
		{
			if (i < lstField.size() - 1)
			{
				strTmp += *it + " ";
			}
			else
			{
				strTmp += *it;
			}
		}
		strCmd = "HMGET " + strHashKey + " " + strTmp;

		Excute(strCmd, cResult, READ);

		if (cResult.GetReplyType() == REPLY_ERROR)
		{
			break;
		}
		else
		{
			lstValue = cResult.GetReplyList();
			bSuccess = True;
		}
	} while (0);

	return bSuccess;
}

void CRedis::Excute(const string& strCmd, CRedisReply & result, ERWFlag flag)
{
//	GenerateParam(strCmd, nArgc, argv);
	list<string> lstCmd;
	STR::Explode(strCmd.c_str(), " ", lstCmd);
	Int32 nArgc = lstCmd.size();

	assert(!lstCmd.empty());
	const char **argv = new const char*[nArgc];
	Int32 nIndex = 0;
	for (list<string>::const_iterator it = lstCmd.begin(); it != lstCmd.end(); it++)
	{
		argv[nIndex++] = (*it).c_str();
	}
	
	do 
	{
		redisContext* pContext = GetConnection();
		//INFO_LOG("Excute(): get connection success, pContext is: " << pContext);

		if (pContext != NULL)
		{
			if (flag == READ)
			{
				ExcuteRead(pContext, nArgc, argv, strCmd, result);
				delete[] argv;
				argv = NULL;
			}
			else if (flag == WRITE)
			{
				ExcuteWrite(pContext, nArgc, argv, strCmd, result);
				delete[] argv;
				argv = NULL;
			}
		}
		else
		{
			result.Clear();
			result.SetReplyErr("All the redis connections unavaliable!");
			result.SetReplyType(REPLY_ERROR);

			ReleaseConnection(pContext);

			delete[] argv;
			argv = NULL;

			//ERROR_LOG("Execute(), all the redis connect unavaliable, cmd is: " << strCmd);
			return;
		}
	} while (0);
}

void CRedis::ExcuteRead(redisContext* pContext, int argc, const char **argv, const string& strCmd, CRedisReply& result)
{
	redisReply* pReply = (redisReply*)redisCommandArgv(pContext, argc, argv, NULL);
	
	do 
	{
		if (pReply != NULL)
		{
			//INFO_LOG("ExcuteRead(): cmd is: " << strCmd);
			//INFO_LOG("ExcuteRead(): pReply is not null----"
			//	<< pReply->type << " | " << pReply->str << " | " << pReply->len << " | " << pReply->integer << " | " << pReply->elements);

			if (pContext->err != REDIS_OK)
			{
				//ERROR_LOG("ExcuteRead(): reply is not null, but err exist" << "cmd is: " << strCmd);
				//ERROR_LOG("errno is: " << pContext->err << ", err is: " << pContext->errstr);
			}
			if (pReply->elements <= 0)
			{
				//WARN_LOG("ExcuteRead(): reply elements is " << pReply->elements);
			}

			// 解析pReply，并释放pReply 
			PraseReply2Result4Read(pReply, result, strCmd);
			freeReplyObject(pReply);
			pReply = NULL;

			// 不管pContext是否仍然正常，直接还回队列中
			ReleaseConnection(pContext);

			return;
		}
		else
		{
			// 执行命令结果为空，不管pContext是否仍然正常，直接还回队列中
			//ERROR_LOG("ExcuteRead(): cmd is: " << strCmd);
			//ERROR_LOG("ExcuteRead(): reply is null, errno is:" << pContext->err << ", err is: " << pContext->errstr);

			ReleaseConnection(pContext);

			return;
		}
	} while (0);
}

void CRedis::ExcuteWrite(redisContext* pContext, int argc, const char **argv, const string& strCmd, CRedisReply& result)
{
	redisReply* pReply = (redisReply*)redisCommandArgv(pContext, argc, argv, NULL);
	do 
	{
		if (pReply != NULL)
		{
			//INFO_LOG("ExcuteWrite(): cmd is: " << strCmd);
			//INFO_LOG("ExcuteWrite(): pReply is not null----"
			//	<< pReply->type << " | " << pReply->str << " | " << pReply->len << " | " << pReply->integer << " | " << pReply->elements);

			if (pContext->err != REDIS_OK)
			{
				//ERROR_LOG("ExcuteWrite(): reply is not null, but err exist" << "cmd is: " << strCmd);
				//ERROR_LOG("errno is: " << pContext->err << ", err is: " << pContext->errstr);
			}

			// 解析pReply，并释放pReply
			PraseReply2Result4Write(pReply, result, strCmd);
			freeReplyObject(pReply);
			pReply = NULL;

			// 不管pContext是否仍然正常，直接还回队列中
			ReleaseConnection(pContext);

			return;
		}
		else
		{
			// 执行命令结果为空，不管pContext是否仍然正常，直接还回队列中
			//ERROR_LOG("ExcuteRead(): cmd is: " << strCmd);
			//ERROR_LOG("ExcuteRead(): reply is null, errno is:" << pContext->err << ", err is: " << pContext->errstr);

			ReleaseConnection(pContext);

			return;
		}
	} while (0);
}

redisContext * CRedis::GetConnection()
{
	redisContext* pContext = NULL;
	m_mtxRedisConnectList.Lock();

	do 
	{
		if (m_lstRedisConnect.empty())
		{
			//WARN_LOG("GetContextFromList(): list is empty");
			m_mtxRedisConnectList.Unlock();
			break;
		}
		else
		{
			pContext = m_lstRedisConnect.front();
			m_lstRedisConnect.pop_front();
			m_mtxRedisConnectList.Unlock();

			if (pContext->err)
			{
				redisFree(pContext);
				pContext = redisConnectWithTimeout(m_addrRedis.GetIPString().c_str(), m_addrRedis.GetPort(), m_tv);
				if (pContext->err)
				{
					redisFree(pContext);
					pContext = NULL;
				}
			}
		}
	} while (0);
		
	return pContext;
}

void CRedis::ReleaseConnection(redisContext * pContext)
{
	if (pContext != NULL)
	{
		m_mtxRedisConnectList.Lock();
		m_lstRedisConnect.push_back(pContext);
		m_mtxRedisConnectList.Unlock();
	}
	else
	{

	}
}

void CRedis::PraseReply2Result4Read(redisReply * pReply, CRedisReply & result, const string & strCmd)
{
	result.Clear();

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		if (strCmd.find("EXISTS") != string::npos || strCmd.find("SISMEMBER") != string::npos || strCmd.find("HEXISTS") != string::npos)
		{
			result.SetReplyType(REPLY_BOOLEN);
			if (pReply->integer == 1)
			{
				result.SetReplyBool(True);
			}
			else
			{
				result.SetReplyBool(False);
			}
		}
		else
		{
			result.SetReplyType(REPLY_INTEGER);
			result.SetReplyInteger(pReply->integer);
		}
	}
	else if (pReply->type == REDIS_REPLY_STRING || pReply->type == REDIS_REPLY_STATUS)
	{
		result.SetReplyType(REPLY_STRING);
		result.SetReplyString(pReply->str);
	}
	else if (pReply->type == REDIS_REPLY_NIL)
	{
		result.SetReplyType(REPLY_STRING);
		result.SetReplyString("");
	}
	else if (pReply->type == REDIS_REPLY_ARRAY)
	{
		if (pReply->elements == 1 && strCmd.find("LRANGE") != string::npos)
		{
			result.SetReplyType(REPLY_STRING);
			result.SetReplyString(pReply->element[0]->str);
		}
		else
		{
			//区分list和map类型
			if (strCmd.find("HGETALL") != string::npos)
			{
				result.SetReplyType(REPLY_HASH);
				result.SetReplyHash(pReply);
			}
			else
			{
				result.SetReplyType(REPLY_LIST);
				result.SetReplyList(pReply);
			}
		}
	}
	else if (pReply->type == REDIS_REPLY_ERROR)
	{
		result.SetReplyType(REPLY_ERROR);
		result.SetReplyErr(pReply->str);
	}
}

void CRedis::PraseReply2Result4Write(redisReply * pReply, CRedisReply & result, const string & strCmd)
{
	result.Clear();

	if (pReply->type == REDIS_REPLY_INTEGER)
	{
		if (strCmd.find("HSET") != string::npos || strCmd.find("SADD") != string::npos)
		{
			result.SetReplyType(REPLY_BOOLEN);
			result.SetReplyBool(True);
		}
		else if (strCmd.find("RPUSH") != string::npos)
		{
			if (pReply->integer < 1)
			{
				result.SetReplyType(REPLY_BOOLEN);
				result.SetReplyErr(False);
			}
			else
			{
				result.SetReplyType(REPLY_BOOLEN);
				result.SetReplyBool(True);
			}
		}
		else
		{
			result.SetReplyType(REPLY_INTEGER);
			result.SetReplyInteger(pReply->integer);
		}
	}
	else if (pReply->type == REDIS_REPLY_STRING || pReply->type == REDIS_REPLY_STATUS)
	{
		result.SetReplyType(REPLY_STRING);
		result.SetReplyString(pReply->str);
	}
	else if (pReply->type == REDIS_REPLY_NIL)
	{
		result.SetReplyType(REPLY_STRING);
		result.SetReplyString("");
	}
	else if (pReply->type == REDIS_REPLY_ARRAY)
	{
		
	}
	else if (pReply->type == REDIS_REPLY_ERROR)
	{
		result.SetReplyType(REPLY_ERROR);
		result.SetReplyErr(pReply->str);
	}
}

map<string, string> CRedis::PraseReplyHash(redisReply * pReply)
{
	map<string, string> mapRes;
	
	for (UInt32 i = 0; i < pReply->elements; i += 2)
	{
		string strField = pReply->element[i]->str;
		string strValue = pReply->element[i + 1]->str;
		mapRes[strField] = strValue;
	}

	return mapRes;
}

/*
list<string> CRedis::PraseReplyList(redisReply * pReply)
{
	list<string> listRes;

	for (UInt32 i = 0; i < pReply->elements; i++)
	{
		if (pReply->element[i]->type == REDIS_REPLY_STRING)
		{
			listRes.push_back(pReply->element[i]->str);
		}
		else if (pReply->element[i]->type == REDIS_REPLY_NIL)
		{
			listRes.push_back("");
		}
	}

	return listRes;
}*/

void CRedis::GenerateParam(const string& strCmd, Int32 nArgc, const char **argv)
{
	list<string> lstCmd;
	STR::Explode(strCmd.c_str(), " ", lstCmd);
	nArgc = lstCmd.size();
	
	assert(!lstCmd.empty());
	argv = new const char*[nArgc];
	Int32 nIndex = 0;
	for (list<string>::const_iterator it = lstCmd.begin(); it != lstCmd.end(); it++)
	{
		argv[nIndex++] = (*it).c_str();
	}
}
