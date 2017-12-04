#ifndef ___HTTP__REQ__MESSAGE__20150512___
#define ___HTTP__REQ__MESSAGE__20150512___

#include "HttpMessage.h"

class CHttpReqMessage : public CHttpMessage
{
public:
	CHttpReqMessage();
	virtual ~CHttpReqMessage();

public:
	Bool SetMessage(const std::string &strHeader, const std::string &strBody);
	virtual Bool IsRequest() const { return True; }
	virtual std::string GetHeader() const;
	virtual std::string GetPacket() const;

	void SetRequestLine(const std::string &strMethod, const std::string &strUrl, const std::string &strProtocol = "HTTP/1.1");
	std::string GetMethod() const;
	std::string GetUri() const;
	std::string GetProtocol() const;

	// GET 参数相关函数
	Bool DoesGetParameterExist(const std::string &strParamter) const;
	std::string GetParameter(const std::string &strParamter) const;
	void SetParameter(const std::string &strParamter, const std::string &strValue);
	void RemoveParameter(const std::string &strParamter);

protected:
	void SetUrl(const std::string &strUrl);
	void ComposeUrl();											// 根据 GET 参数组合 Url

protected:
	std::string							m_strMethod;			// 请求方法，如: GET/POST/HEAD/CONNECT 等
	std::string							m_strUri;				// 请求的 URI, 不包含 ? 后面的参数，如: /
	std::string							m_strProtocol;			// 请求的协议版本，如: HTTP/1.1

	std::string							m_strUrl;				// 请求的完整 Url, 包含参数，如: /?id=100
	std::map<std::string, std::string>	m_mapParameters;		// GET 参数列表
};

#endif
