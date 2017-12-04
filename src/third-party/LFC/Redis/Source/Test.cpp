/***********************************************************************
* Author:      ruanyanfang
* Date:        2017-03-22
* Description: 测试功能使用
***********************************************************************/
#include <time.h>
#include "Common/Common.h"
#include "Log/Log.h"
#include "../include/redis.h"
using namespace std;

void test(CRedis* pRedis)
{
	Int32 nFailed = 0;
	Int32 nSuccess = 0;
	Int32 nBegin = 0;
	Int32 nEnd = 0;
	Int32 nErrStart = 0;
	Bool  nLastErr = False;
	Bool  bLastFlag = False;

	//////////////// SET ////////////////////////////////////
	printf("===== SET =====\n");
	SString sTmp;
	nBegin = OS::Milliseconds();
	for (Int32 i = 0; i < 1000000; i++)
	{
		sTmp.strKey = OS::CreateUUID2();
		sTmp.strValue = sTmp.strKey;
		if (!pRedis->Set(sTmp))
		{
			nFailed++;
			if (bLastFlag)
			{
				bLastFlag = False;
				printf("failed time: %s\n", STR::NumberToString(OS::Milliseconds()).c_str());
			}
		}
		else
		{
			nSuccess++;
			if (!bLastFlag)
			{
				bLastFlag = True;
				printf("conneted to redis server: %ld\n", OS::Milliseconds());
			}
		}
	}
	nEnd = OS::Milliseconds();
	printf("cost: %s ms\n", STR::NumberToString(nEnd - nBegin).c_str());
	printf("failed: %d\n", nFailed);

	//////////////// GET ////////////////////////////////////
	/*printf("===== GET =====\n");
	nFailed = 0;
	nBegin = OS::Milliseconds();
	for (Int32 i = 0; i < 100000; i++)
	{
		sTmp.strKey = OS::CreateUUID2();
		if (!pRedis->Get(sTmp))
		{
			nFailed++;
		}
	}
	nEnd = OS::Milliseconds();
	printf("cost: %s ms\n", STR::NumberToString(nEnd - nBegin).c_str());
	printf("failed: %d\n", nFailed);*/
	
	//////////////// LPUSH ////////////////////////////////////
	/*printf("===== LPUSH =====\n");
	nFailed = 0;
	SList sList;
	nBegin = OS::Milliseconds();
	sList.strKey = "ruan_list";
	for (Int32 i = 0; i < 100000; i++)
	{
		sList.lstData.clear();
		sList.lstData.push_back(OS::CreateUUID2());
		if (!pRedis->PushFront(sList))
		{
			nFailed++;
		}
	}
	nEnd = OS::Milliseconds();
	printf("cost: %s ms\n", STR::NumberToString(nEnd - nBegin).c_str());
	printf("failed: %d\n", nFailed);*/

	//////////////// SADD ////////////////////////////////////
	/*printf("===== SADD =====\n");
	nFailed = 0;
	SSet sSet;
	nBegin = OS::Milliseconds();
	sSet.strKey = "ruan_set";
	for (Int32 i = 0; i < 100000; i++)
	{
		sSet.vecData.clear();
		sSet.vecData.push_back(OS::CreateUUID2());
		if (!pRedis->Set(sSet))
		{
			nFailed++;
		}
	}
	nEnd = OS::Milliseconds();
	printf("cost: %s ms\n", STR::NumberToString(nEnd - nBegin).c_str());
	printf("failed: %d\n", nFailed);*/
	
	//////////////// LRANGE_N ////////////////////////////////////
	/*printf("===== LRANGE_N =====\n");
	nFailed = 0;
	SList sList;
	nBegin = OS::Milliseconds();
	sList.strKey = "ruan_list";
	for (Int32 i = 0; i < 100000; i++)
	{
		if (!pRedis->GetByRange(sList, 0, 449))
		{
			nFailed++;
		}
	}
	nEnd = OS::Milliseconds();
	printf("cost: %s ms\n", STR::NumberToString(nEnd - nBegin).c_str());
	printf("failed: %d\n", nFailed);*/

	//////////////// MSET ////////////////////////////////////
	/*printf("===== MSET =====\n");
	nFailed = 0;
	SHash sHash;
	string strTmp;
	nBegin = OS::Milliseconds();
	sHash.strKey = "ruan_hash";
	for (Int32 i = 0; i < 10; i++)
	{
		strTmp = OS::CreateUUID2();
		sHash.mapData.insert(pair<string, string>(strTmp, strTmp));
	}
	for (Int32 i = 0; i < 100000; i++)
	{
		if (!pRedis->Set(sHash))
		{
			nFailed++;
		}
	}
	nEnd = OS::Milliseconds();
	printf("cost: %s ms\n", STR::NumberToString(nEnd - nBegin).c_str());
	printf("failed: %d\n", nFailed);*/
}

Int32 main(Int32 argc, LPSTR argv[])
{
	do 
	{
		CEventEngine engine;
		if (!engine.Initialize())
		{
			printf("Failed to initialize engine.");
			break;
		}

		CEventThread* pRedisThread = new CEventThread();
		if (!pRedisThread->Initialize(22))
		{
			printf("Failed to initialize redis thread on cpu22.");
			break;
		}				

		string strLogFolder = OS::GetModuleFilePath() + "./log";
		if (!OS::MakeDirectory(strLogFolder.c_str()))
		{
			printf("Failed to create log directory.\n");
			break;
		}
		OS::NormalizeFileName(strLogFolder);
		string strSysLogFile = strLogFolder + "syslog.log";
		string strRunLogFile = strLogFolder + "runlog.log";
		CLog *pLog = CSingleton<CLog>::GetInstancePtr();
		if (!pLog->Initialize(strSysLogFile.c_str(), 50 * 1000 * 1000, 10, strRunLogFile.c_str(), 50 * 1000 * 1000, 10, False))
		{
			printf("Failed to initialize log system.\n");
			break;
		}
		pLog->SetSysLogLevel(0);
		pLog->SetRunLogLevel(0);

		CRedis* pRedis = CSingleton<CRedis>::GetInstancePtr();
		SRedisInfo sRedis;
		sRedis.nConnectNum = 1;
		sRedis.redisAddr.SetAddress("172.20.100.110:6379");
		sRedis.tv.tv_sec = 10;
		sRedis.tv.tv_usec = 0;
		if (pRedis->Initialize(sRedis, pRedisThread))
		{
			test(pRedis);
		}
		else
		{
			printf("Failed to init redis.\n");
		}

		while (1)
		{
			engine.Loop(500);
		}
	} while (0);

	return 0;
}




