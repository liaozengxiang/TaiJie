#ifndef HTTP__CLIENT__EVENT__20160401
#define HTTP__CLIENT__EVENT__20160401

#include <Framework/ClientEvent.h>
#include "HttpParser.h"
#include "HttpReqMessage.h"
#include "HttpResMessage.h"
#include "HttpCode.h"

struct IHttpHandler
{
	/*
	 * 功能: 执行完请求后，框架请求删除处理器对象
	 * 说明: 
	 *       框架通过抽象工厂 IHttpHandlerFactory 产生 IHttpHandler 的派生类对象
	 *       上层可以控制创建对象的方法，如对象池、new、malloc等
	 *       框架通过 Release 方法将 IHttpHandler 派生类对象的删除延迟到子类执行
	 */
	virtual void Release() = 0;

	/*
	 * 功能: 此函数为框架调用的第一个函数，在调用 OnCheckRequest 函数之前
	 * 参数: 
	 *       @addr      客户端地址
	 *       @nFD       客户端套接字
	 *       @pMsg      已解析的客户端请求报文
	 * 返回: 
	 *       返回 True,  用来告诉框架继续按流程处理该请求
	 *       返回 False, 框架会解除套接字的绑定，该套接字上的 I/O 事件由上层处理，后续只会调用 Release
	 */
	virtual Bool OnPreCheckRequest(const CSocketAddr &addr, Int32 nFD, const CHttpReqMessage *pMsg) = 0;

	/*
	 * 功能: 框架调用的第二个函数，用于检查 Http 请求的合法性
	 * 参数: 
	 *       @pMsg      客户端请求消息
	 * 返回: 
	 *       检查请求参数成功时返回 eHttpOK
	 *       否则返回其他 Http 错误码，此后框架不会调用 OnHandleRequest
	 */
	virtual EHttpCode OnCheckRequest(const CHttpReqMessage *pMsg) = 0;

	/*
	 * 功能: 框架调用的第三个函数，用于处理客户端请求，并返回响应消息
	 * 参数: 
	 *       @pRequestMsg   客户端请求消息
	 *       @pResponseMsg  服务端响应消息
	 * 返回: N/A
	 */
	virtual void OnHandleRequest(const CHttpReqMessage *pRequestMsg, CHttpResMessage *pResponseMsg) = 0;

	/*
	 * 功能: 框架调用的最后一个函数，在将响应消息发送给客户端之前时调用
	 * 参数: 
	 *       @pMsg     待响应给客户端的消息
	 * 返回: N/A
	 */
	virtual void OnPreResponse(CHttpResMessage *pMsg) = 0;
};

struct IHttpHandlerFactory
{
	virtual IHttpHandler* Create(const CHttpReqMessage *pMsg) = 0;
};

class CHttpClientEvent : public CClientEvent
{
public:
	CHttpClientEvent(Int32 nFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher, IHttpHandlerFactory *pHandlerFactory, Int64 lLinkTimeout);
	virtual ~CHttpClientEvent();

protected:
	// CClientEvent
	virtual Bool Parse(LPCSTR lpszBuffer, Int32 nLen);
	virtual void HandleMessage(void *pMsg, Bool bAsync);
	virtual void OnMessageSent();
	
	// CEvent
	virtual void OnTimer(CTimer *pTimer);

protected:
    static int on_message_begin(http_parser *parser);
    static int on_url(http_parser*, const char *at, size_t length);
    static int on_header_field(http_parser*, const char *at, size_t length);
    static int on_header_value(http_parser*, const char *at, size_t length);
    static int on_headers_complete(http_parser *parser);
    static int on_body(http_parser *parser, const char *at, size_t length);
    static int on_message_complete(http_parser *parser);

    Int32 OnMessageBegin();
    Int32 OnUrl(LPCSTR at, Int32 nLen);
    Int32 OnHeaderField(LPCSTR at, Int32 nLen);
    Int32 OnHeaderValue(LPCSTR at, Int32 nLen);
    Int32 OnHeadersComplete();
    Int32 OnBody(LPCSTR at, Int32 nLen);
    Int32 OnMessageComplete();

protected:
	IHttpHandlerFactory*				m_pHandlerFactory;

    // 解析 http 头的状态
    enum EParseState
    {
        eParseNone = 0,
        eParseField,
        eParseValue
    };

	http_parser                         m_parser;
    http_parser_settings                m_settings;
    EParseState                         m_eParseState;              // 解析 HTTP 头部的状态

    std::string                         m_strUrl;                   // HTTP 的请求地址
    std::string                         m_strHeader;                // HTTP 头部数据，只包括 Field: Value 组列表
    std::string                         m_strBody;                  // HTTP 消息体，由解析器解析并拼凑而成，为完整的正文

	Bool								m_bKeepAlive;
	Int64								m_lLinkTimeout;				// 服务端响应 Keep-Alived 时后续的链接超时
	Int64								m_lLinkActiveTime;			// 链接上一次活动时间
};

#endif
