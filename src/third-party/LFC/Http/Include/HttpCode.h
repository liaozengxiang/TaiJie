#ifndef HTTP__CODE__20160402
#define HTTP__CODE__20160402

#include <Common/Common.h>

enum EHttpCode
{
	eHttpContinue                     = 100,
	eHttpSwitchingProtocols           = 101,
	eHttpProcessing                   = 102,
	eHttpOK                           = 200,
	eHttpCreated                      = 201,
	eHttpAccepted                     = 202,
	eHttpNonAuthoriativeInformation   = 203,
	eHttpNoContent                    = 204,
	eHttpResetContent                 = 205,
	eHttpPartialContent               = 206,
	eHttpMultiStatus                  = 207,
	eHttpMultipleChoices              = 300,
	eHttpMovedPermanently             = 301,
	eHttpFound                        = 302,
	eHttpSeeOther                     = 303,
	eHttpNotModified                  = 304,
	eHttpUseProxy                     = 305,
	eHttpUnused                       = 306,
	eHttpTemporaryRedirect            = 307,
	eHttpBadRequest                   = 400,
	eHttpUnauthorized                 = 401,
	eHttpPaymentGranted               = 402,
	eHttpForbidden                    = 403,
	eHttpFileNotFound                 = 404,
	eHttpMethodNotAllowed             = 405,
	eHttpNotAcceptable                = 406,
	eHttpProxyAuthenticationRequired  = 407,
	eHttpRequestTimeout               = 408,
	eHttpConflict                     = 409,
	eHttpGone                         = 410,
	eHttpLengthRequired               = 411,
	eHttpPreconditionFailed           = 412,
	eHttpRequestEntityTooLarge        = 413,
	eHttpRequestURITooLarge           = 414,
	eHttpUnsupportedMediaType         = 415,
	eHttpRequestedRangeNotSatisfiable = 416,
	eHttpExpectationFailed            = 417,
	eHttpUnprocessableEntity          = 422,
	eHttpLocked                       = 423,
	eHttpFailedDependency             = 424,
	eHttpInternalServerError          = 500,
	eHttpNotImplemented               = 501,
	eHttpBadGateway                   = 502,
	eHttpServiceUnavailable           = 503,
	eHttpGatewayTimeout               = 504,
	eHttpHTTPVersionNotSupported      = 505,
	eHttpInsufficientStorage          = 507
};

class CHttpCode
{
	DECLARE_SINGLETON(CHttpCode)

private:
	CHttpCode();
	~CHttpCode();

public:
	std::string GetHttpCodeDesc(EHttpCode eCode) const;

private:
	std::map<EHttpCode, std::string> m_mapHttpCodeDesc;
};

#endif
