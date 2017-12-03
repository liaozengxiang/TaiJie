#include <Http/HttpFrameServer.h>
#include <Log/Log.h>

class CHttpDefaultHandler : public IHttpHandler
{
protected:
    virtual void Release()
    {
        delete this;
    }

    virtual Bool OnPreCheckRequest(const CSocketAddr &addr, Int32 nFD, const CHttpReqMessage *pMsg)
    {
        INFO_LOG("Recv from " << addr.GetAddrString().c_str() << "\n" << pMsg->GetPacket());

        return True;
    }

    virtual EHttpCode OnCheckRequest(const CHttpReqMessage *pMsg)
    {
        return eHttpOK;
    }

    virtual void OnHandleRequest(const CHttpReqMessage *pRequestMsg, CHttpResMessage *pResponseMsg)
    {
    }

    virtual void OnPreResponse(CHttpResMessage *pMsg)
    {
        pMsg->SetHeaderField("Server", "Test Server/1.0");
        pMsg->SetHeaderField("Date", OS::GreenwichMeanTime3());
        pMsg->SetHeaderField("Connection", "close");

        INFO_LOG("Send to client: \n" << pMsg->GetPacket());
    }
};

class CHttpNotFoundHandler : public CHttpDefaultHandler
{
    virtual EHttpCode OnCheckRequest(const CHttpReqMessage *pMsg)
    {
        return eHttpFileNotFound;
    }
};

class CHttpHomeHandler : public CHttpDefaultHandler
{
    virtual void OnHandleRequest(const CHttpReqMessage *pRequestMsg, CHttpResMessage *pResponseMsg)
    {
        pResponseMsg->SetBody("<h1>Hello, world!</h1>\n<h2>This is the home page!</h2>", "text/html");
    }
};

class CHttpHandlerFactory : public IHttpHandlerFactory
{
    virtual IHttpHandler* Create(const CHttpReqMessage *pMsg)
    {
        const std::string strUri = pMsg->GetUri();
        IHttpHandler *pHandler = NULL;
        if (strUri == "/")
        {
            pHandler = new CHttpHomeHandler();
        }
        else
        {
            pHandler = new CHttpNotFoundHandler();
        }

        return pHandler;
    }
};

#include <sys/types.h>
#include <unistd.h>
#include <iostream>

void PrintEnv()
{
    extern char **environ;
    for (Int32 i = 0; environ[i] != NULL; ++i)
    {
        std::cout << "pid[" << getpid() << "] env[" << i << "]: " << environ[i] << std::endl;
    }

    std::cout << std::endl;
}

Bool bExit = False;
void OnExit(Int32 nSignal)
{
    bExit = True;
}

class CTestThread : public CThread
{
protected:
	virtual void OnRun()
	{
		while (1)
		{
			INFO_LOG("This message comes from thread");
		}
	}
};

Int32 main(Int32 argc, LPSTR argv[])
{
    OS::RegisterSignal(SIGINT, OnExit);

	CLog *pLog = CSingleton<CLog>::GetInstancePtr();
	if (!pLog->Initialize("syslog.log", 10*1024*1024, 10, "runlog.log", 10*1024*1024, 2, False))
	{
		return 0;
	}

	pLog->SetSysLogLevel(1);
	pLog->SetRunLogLevel(1);

    CEventEngine engine;
    engine.Initialize(32);

    std::vector<CEventEngine*> vecAsyncIo;
    for (Int32 i = 0; i < 8; ++i)
    {
        CEventEngine *pEngine = new CEventThread();
        pEngine->Initialize(i);
        vecAsyncIo.push_back(pEngine);
    }

    //	CThreadPool workPool;
    //	workPool.Start(8, 0xFFFFFFFF);

    CHttpHandlerFactory handlerFactory;
    CHttpFrameServer server(&engine);
    server.SetLinkTimeout(30000);
    //	server.Start("0.0.0.0:80", vecAsyncIo, &workPool, &handlerFactory);
    server.Start("0.0.0.0:80", vecAsyncIo, NULL, &handlerFactory);

    while (!bExit)
    {
        engine.Loop(500);
    }

    for (std::vector<CEventEngine*>::const_iterator it = vecAsyncIo.begin(); it != vecAsyncIo.end(); ++it)
    {
        delete *it;
    }

    return 0;
}

class CWatch : public IFileWatchEvent
{
    virtual void OnFileWatch(const std::string &strBaseName, const std::string &strFileName, UInt32 uMask, Bool bIsDirectory)
    {
        printf("Mask: %08X, BaseName: %s, File: %s, IsDirectory: %d\n", uMask, strBaseName.c_str(), strFileName.c_str(), bIsDirectory);
    }
};


using namespace tinyxml2;

Int32 main2(Int32 argc, LPSTR argv[])
{
    do
    {
        CLog *pLog = CSingleton<CLog>::GetInstancePtr();
        if (!pLog->Initialize("syslog.log", 10*1024*1024, 2, "runlog.log", 10*1024*1024, 10, True))
        {
            break;
        }

        pLog->SetSysLogLevel(1);
        pLog->SetRunLogLevel(1);

        std::ostringstream oss;
        oss << "<Config>"
            << "    <Name age=\"28\">Tony</Name>"
            << "    <Item age=\"26\">Jimmy</Item>"
            << "    <Name age=\"30\">Lory Liao</Name>"
            << "</Config>";

        CXmlParser xml;
        if (!xml.LoadContent(oss.str().c_str()))
        {
            ERROR_LOG("Failed to parse xml");
            break;
        }

        std::string strValue;
        if (!xml.GetContent("Config/Item", strValue))
        {
            ERROR_LOG("Failed to get_content");
            break;
        }
        INFO_LOG("Name is: " << strValue);

        Int32 nAge;
        if (!xml.GetAttribute("Config/Name&0", "age", nAge))
        {
            ERROR_LOG("Failed to get_attribute");
            break;
        }
        INFO_LOG("XML age is: " << nAge);

        Int32 nCount = xml.GetChildNum("Config");
        INFO_LOG("Sub node count is: " << nCount);

        std::list<std::string> lstItems;
        std::list<std::string>::iterator it1;
        if (!xml.GetContent("Config/Name", lstItems))
        {
            ERROR_LOG("Failed to get_content for list");
            break;
        }
        DEBUG_LOG("list item count is: " << lstItems.size());
        for (it1 = lstItems.begin(); it1 != lstItems.end(); ++it1)
        {
            DEBUG_LOG("list_item: " << *it1);
        }

        std::vector<std::string> vecItems;
        std::vector<std::string>::iterator it2;
        if (!xml.GetContent("Config/Name", vecItems))
        {
            ERROR_LOG("Failed to get_content for vector");
            break;
        }
        DEBUG_LOG("vector item count is: " << vecItems.size());
        for (it2 = vecItems.begin(); it2 != vecItems.end(); ++it2)
        {
            DEBUG_LOG("vector_item: " << *it2);
        }


        break;
        
        /*
        XMLDocument doc;
        XMLError error = doc.Parse(oss.str().c_str());
        if (error == XML_NO_ERROR)
        {
            INFO_LOG("Parse xml OK");
            INFO_LOG(doc.FirstChildElement("Config")->FirstChildElement("Name")->Attribute("value"));
        }
        else
        {
            ERROR_LOG("Parse xml failed. Error=" << error << ":" << doc.GetErrorStr1() << ":" << doc.GetErrorStr2());
        }
        */
    } while (0);

    return 0;
}
