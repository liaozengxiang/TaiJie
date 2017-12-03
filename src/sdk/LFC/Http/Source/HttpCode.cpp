#include "../Include/HttpCode.h"


CHttpCode::CHttpCode()
{
	m_mapHttpCodeDesc[eHttpContinue]                     = "Continue";
	m_mapHttpCodeDesc[eHttpSwitchingProtocols]           = "Switching Protocols";
	m_mapHttpCodeDesc[eHttpProcessing]                   = "Processing";
	m_mapHttpCodeDesc[eHttpOK]                           = "OK";
	m_mapHttpCodeDesc[eHttpCreated]                      = "Created";
	m_mapHttpCodeDesc[eHttpAccepted]                     = "Accepted";
	m_mapHttpCodeDesc[eHttpNonAuthoriativeInformation]   = "Non-Authoriative Information";
	m_mapHttpCodeDesc[eHttpNoContent]                    = "No Content";
	m_mapHttpCodeDesc[eHttpResetContent]                 = "Reset Content";
	m_mapHttpCodeDesc[eHttpPartialContent]               = "Partial Content";
	m_mapHttpCodeDesc[eHttpMultiStatus]                  = "Multi-Status";
	m_mapHttpCodeDesc[eHttpMultipleChoices]              = "Multiple Choices";
	m_mapHttpCodeDesc[eHttpMovedPermanently]             = "Moved Permanently";
	m_mapHttpCodeDesc[eHttpFound]                        = "Found";
	m_mapHttpCodeDesc[eHttpSeeOther]                     = "See Other";
	m_mapHttpCodeDesc[eHttpNotModified]                  = "Not Modified";
	m_mapHttpCodeDesc[eHttpUseProxy]                     = "Use Proxy";
	m_mapHttpCodeDesc[eHttpUnused]                       = "(Unused)";
	m_mapHttpCodeDesc[eHttpTemporaryRedirect]            = "Temporary Redirect";
	m_mapHttpCodeDesc[eHttpBadRequest]                   = "Bad Request";
	m_mapHttpCodeDesc[eHttpUnauthorized]                 = "Unauthorized";
	m_mapHttpCodeDesc[eHttpPaymentGranted]               = "Payment Granted";
	m_mapHttpCodeDesc[eHttpForbidden]                    = "Forbidden";
	m_mapHttpCodeDesc[eHttpFileNotFound]                 = "File Not Found";
	m_mapHttpCodeDesc[eHttpMethodNotAllowed]             = "Method Not Allowed";
	m_mapHttpCodeDesc[eHttpNotAcceptable]                = "Not Acceptable";
	m_mapHttpCodeDesc[eHttpProxyAuthenticationRequired]  = "Proxy Authentication Required";
	m_mapHttpCodeDesc[eHttpRequestTimeout]               = "Request Time-out";
	m_mapHttpCodeDesc[eHttpConflict]                     = "Conflict";
	m_mapHttpCodeDesc[eHttpGone]                         = "Gone";
	m_mapHttpCodeDesc[eHttpLengthRequired]               = "Length Required";
	m_mapHttpCodeDesc[eHttpPreconditionFailed]           = "Precondition Failed";
	m_mapHttpCodeDesc[eHttpRequestEntityTooLarge]        = "Request Entity Too Large";
	m_mapHttpCodeDesc[eHttpRequestURITooLarge]           = "Request-URI Too Large";
	m_mapHttpCodeDesc[eHttpUnsupportedMediaType]         = "Unsupported Media Type";
	m_mapHttpCodeDesc[eHttpRequestedRangeNotSatisfiable] = "Requested Range Not Satisfiable";
	m_mapHttpCodeDesc[eHttpExpectationFailed]            = "Expectation Failed";
	m_mapHttpCodeDesc[eHttpUnprocessableEntity]          = "Unprocessable Entity";
	m_mapHttpCodeDesc[eHttpLocked]                       = "Locked";
	m_mapHttpCodeDesc[eHttpFailedDependency]             = "Failed Dependency";
	m_mapHttpCodeDesc[eHttpInternalServerError]          = "Internal Server Error";
	m_mapHttpCodeDesc[eHttpNotImplemented]               = "Not Implemented";
	m_mapHttpCodeDesc[eHttpBadGateway]                   = "Bad Gateway";
	m_mapHttpCodeDesc[eHttpServiceUnavailable]           = "Service Unavailable";
	m_mapHttpCodeDesc[eHttpGatewayTimeout]               = "Gateway Timeout";
	m_mapHttpCodeDesc[eHttpHTTPVersionNotSupported]      = "HTTP Version Not Supported";
	m_mapHttpCodeDesc[eHttpInsufficientStorage]          = "Insufficient Storage";
}

CHttpCode::~CHttpCode()
{
}

std::string CHttpCode::GetHttpCodeDesc(EHttpCode eCode) const
{
	std::map<EHttpCode, std::string>::const_iterator it = m_mapHttpCodeDesc.find(eCode);
	if (it != m_mapHttpCodeDesc.end())
	{
		return it->second;
	}

	return "Unknown";
}
