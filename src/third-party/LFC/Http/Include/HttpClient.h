/*
| -------------------------------------------------------------------------
| CHttpClient: Http 客户端请求类
| -------------------------------------------------------------------------
| 通过异步事件驱动机制实现 Http 客户端类请求->响应
|
| 注意: CHttpClient 类的对象必须创建为智能指针，否则回调函数不会被调用
| 示例: 
|      CSharedPtr<CHttpClient> spClient(new CHttpClient(pEngine));
|      spClient->Request("127.0.0.1:80", callback);
*/

#ifndef HTTP__CLIENT__H__20170522
#define HTTP__CLIENT__H__20170522

#include <string>
#include <list>
#include <Common/Common.h>
using namespace std;

struct IHttpClientRequest
{
    virtual ~IHttpClientRequest() { }
    virtual Bool OnHttpClientSend(string &str)                   = 0;
    virtual void OnHttpClientRecv(LPCSTR lpszBuffer, Int32 nLen) = 0;
    virtual void OnHttpClientClosed(Int32 nError)                = 0;
};

class CHttpClient : public ITCPConnectorCallback, public CEnableSharedFromThis<CHttpClient>, public CEvent
{
public:
    CHttpClient(CEventEngine *pEngine);
    virtual ~CHttpClient();

public:
    Bool Request(const CSocketAddr &addr, CWeakPtr<IHttpClientRequest> wpCallback, Int32 nTimeout = -1);

protected:
    virtual void OnConnected();
    virtual void OnConnectFailed(Int32 nError);
    virtual void OnConnectTimeout();

    virtual Int32 GetFD() const { return m_nFD; }
    virtual void OnRead();
    virtual void OnWrite();
    virtual void OnError(Int32 nErrCode);

protected:
    void FillSendData();

protected:
    CTCPConnector                       m_connector;
    Int32                               m_nFD;

    CWeakPtr<IHttpClientRequest>        m_wpCallback;
    list<CBuffer*>                      m_lstBuffers;
};

#endif
