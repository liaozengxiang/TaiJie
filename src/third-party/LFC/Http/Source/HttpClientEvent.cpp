#include <string.h>
#include <sstream>
#include "../Include/HttpClientEvent.h"

CHttpClientEvent::CHttpClientEvent(Int32 nFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher, IHttpHandlerFactory *pHandlerFactory, Int64 lLinkTimeout)
	: CClientEvent(nFD, addr, pEngine, pDispatcher)
{
	Assert(pHandlerFactory != NULL);

	m_pHandlerFactory = pHandlerFactory;
    
	m_bKeepAlive      = True;
	m_lLinkTimeout    = lLinkTimeout;
	m_lLinkActiveTime = OS::Milliseconds();

    // 初始化解析器
    http_parser_init(&m_parser, HTTP_REQUEST);
    m_parser.data = this;
    m_eParseState = eParseNone;

    memset(&m_settings, 0, sizeof(m_settings));
    m_settings.on_message_begin    = &CHttpClientEvent::on_message_begin;
    m_settings.on_url              = &CHttpClientEvent::on_url;
    m_settings.on_header_field     = &CHttpClientEvent::on_header_field;
    m_settings.on_header_value     = &CHttpClientEvent::on_header_value;
    m_settings.on_headers_complete = &CHttpClientEvent::on_headers_complete;
    m_settings.on_body             = &CHttpClientEvent::on_body;
    m_settings.on_message_complete = &CHttpClientEvent::on_message_complete;
}

CHttpClientEvent::~CHttpClientEvent()
{
}

Bool CHttpClientEvent::Parse(LPCSTR lpszBuffer, Int32 nLen)
{
	m_lLinkActiveTime = OS::Milliseconds();
    return (http_parser_execute(&m_parser, &m_settings, lpszBuffer, nLen) == (size_t)nLen);
}

void CHttpClientEvent::HandleMessage(void *pMsg, Bool bAsync)
{
	// 此函数被调用时，有二种可能
	// (1) 上层启动服务器未指定工作线程池，则该函数由对应的 I/O 线程同步调用
	// (2) 上层启动服务器指定了工作线程池，则该函数由工作线程池调用，且调用之前已对 CHttpClientEvent 增加引用计数

	// 取出 Http 请求消息
	CHttpReqMessage *pReqMsg = static_cast<CHttpReqMessage *>(pMsg);

	Assert(pReqMsg != NULL);
	CAutoPtr<CHttpReqMessage> ptrDel(pReqMsg);

	// 创建消息处理器对象
	IHttpHandler *pHandler = m_pHandlerFactory->Create(pReqMsg);
	Assert(pHandler != NULL);

	do
	{
        if (!bAsync)
        {
            Unregister();
        }

		if (!pHandler->OnPreCheckRequest(m_address, m_nFD, pReqMsg))
		{
			Detach(bAsync);
			break;
		}

        if (!bAsync)
        {
            RegisterRead();
        }

		// 检查请求消息是否正确
		EHttpCode eCode = pHandler->OnCheckRequest(pReqMsg);

		// 创建 Http 响应对象
		CHttpResMessage resMsg;
		resMsg.SetCode(eCode, CSingleton<CHttpCode>::GetInstancePtr()->GetHttpCodeDesc(eCode), "HTTP/1.1");

		if (eCode == eHttpOK)
		{
			// 只有在检查消息返回成功时才执行请求
			pHandler->OnHandleRequest(pReqMsg, &resMsg);
		}

		pHandler->OnPreResponse(&resMsg);
		Send(resMsg.GetPacket(), bAsync);

		// 如果响应中指定了 "Connection: close", 则响应完毕后释放连接
		m_bKeepAlive = True;
		if (STR::InsensitiveCompare(resMsg.GetHeaderField("Connection").c_str(), "close") == 0)
		{
			m_bKeepAlive = False;
		}

	} while (0);

	// 删除创建的处理器对象
	pHandler->Release();
}

void CHttpClientEvent::OnMessageSent()
{
	if (m_bKeepAlive)
	{
		RegisterRead();
	}
	else
	{
		Unregister();
		Close();
	}
}

void CHttpClientEvent::OnTimer(CTimer *pTimer)
{
	if (OS::Milliseconds() - m_lLinkActiveTime >= m_lLinkTimeout)
	{
		Unregister();
		Close();
	}

	CClientEvent::OnTimer(pTimer);
}

int CHttpClientEvent::on_message_begin(http_parser *parser)
{
    CHttpClientEvent *pInstance = (CHttpClientEvent *)parser->data;
    return pInstance->OnMessageBegin();
}

int CHttpClientEvent::on_url(http_parser *parser, const char *at, size_t length)
{
    CHttpClientEvent *pInstance = (CHttpClientEvent *)parser->data;
    return pInstance->OnUrl(at, length);
}

int CHttpClientEvent::on_header_field(http_parser *parser, const char *at, size_t length)
{
    CHttpClientEvent *pInstance = (CHttpClientEvent *)parser->data;
    return pInstance->OnHeaderField(at, length);
}

int CHttpClientEvent::on_header_value(http_parser *parser, const char *at, size_t length)
{
    CHttpClientEvent *pInstance = (CHttpClientEvent *)parser->data;
    return pInstance->OnHeaderValue(at, length);
}

int CHttpClientEvent::on_headers_complete(http_parser *parser)
{
    CHttpClientEvent *pInstance = (CHttpClientEvent *)parser->data;
    return pInstance->OnHeadersComplete();
}

int CHttpClientEvent::on_body(http_parser *parser, const char *at, size_t length)
{
    CHttpClientEvent *pInstance = (CHttpClientEvent *)parser->data;
    return pInstance->OnBody(at, length);
}

int CHttpClientEvent::on_message_complete(http_parser *parser)
{
    CHttpClientEvent *pInstance = (CHttpClientEvent *)parser->data;
    return pInstance->OnMessageComplete();
}

Int32 CHttpClientEvent::OnMessageBegin()
{
    m_eParseState = eParseNone;
    m_strUrl.clear();
    m_strHeader.clear();
    m_strBody.clear();

    return 0;
}

Int32 CHttpClientEvent::OnUrl(LPCSTR at, Int32 nLen)
{
    m_strUrl.append(at, nLen);
    return 0;
}

Int32 CHttpClientEvent::OnHeaderField(LPCSTR at, Int32 nLen)
{
    switch (m_eParseState)
    {
    case eParseNone:
        m_eParseState = eParseField;
        m_strHeader.append(at, nLen);
        break;

    case eParseField:
        m_strHeader.append(at, nLen);
        break;

    case eParseValue:
        m_eParseState = eParseField;
        m_strHeader.append("\r\n");
        m_strHeader.append(at, nLen);
        break;
    }

    return 0;
}

Int32 CHttpClientEvent::OnHeaderValue(LPCSTR at, Int32 nLen)
{
    switch (m_eParseState)
    {
    case eParseNone:
        Assert(False);
        break;

    case eParseField:
        m_eParseState = eParseValue;
        m_strHeader.append(": ");
        m_strHeader.append(at, nLen);
        break;

    case eParseValue:
        m_strHeader.append(at, nLen);
        break;
    }

    return 0;
}

Int32 CHttpClientEvent::OnHeadersComplete()
{
    m_strHeader.append("\r\n\r\n");
    return 0;
}

Int32 CHttpClientEvent::OnBody(LPCSTR at, Int32 nLen)
{
    m_strBody.append(at, nLen);
    return 0;
}

Int32 CHttpClientEvent::OnMessageComplete()
{
    // 定义请求方法的索引表
    static LPCSTR lpszMethod[256] = { NULL };
    if (unlikely(lpszMethod[1] == NULL))
    {
#define XX(num, name, str) lpszMethod[num] = #str;
    HTTP_METHOD_MAP(XX)
#undef XX
    }

    // 请求协议
    char szProtocol[16];
    sprintf(szProtocol, "HTTP/%d.%d", m_parser.http_major, m_parser.http_minor);

    // 将 HTTP 完整头进行合并
    std::ostringstream oss;
    oss << lpszMethod[m_parser.method] << " " << m_strUrl << " " << szProtocol << "\r\n" << m_strHeader;
    std::string strHeader = oss.str();

    // 该函数由套接字 I/O 线程调用
    // 由于消息处理器可能是线程池，有可能在之前的消息请求过程中关闭了连接，此时该请求可以忽略
    do
    {
        if (-1 == m_nFD)
        {
            break;
        }

        // 创建请求消息
        CHttpReqMessage *pMsg = new CHttpReqMessage();
        if (!pMsg->SetMessage(strHeader, m_strBody))
        {
            // 请求的消息无法正常解析，直接关闭连接
            delete pMsg;
            Unregister();
            Close();
            break;
        }

        // 派遣消息
        DispatchMessage(pMsg);
    } while (0);

    return 0;
}
