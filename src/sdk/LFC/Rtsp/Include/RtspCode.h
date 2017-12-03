#ifndef RTSP__CODE__20160402
#define RTSP__CODE__20160402

#include <Common/Common.h>

enum ERtspCode
{
    eRtspContinue                       = 100,
    eRtspOK                             = 200,
    eRtspCreated                        = 201,
    eRtspLowOnStorageSpace              = 250,
    eRtspMultipleChoices                = 300,
    eRtspMovedPermanently               = 301,
    eRtspMovedTemporarily               = 302,
    eRtspSeeOther                       = 303,
    eRtspNotModified                    = 304,
    eRtspUseProxy                       = 305,
    eRtspBadRequest                     = 400,
    eRtspUnauthorized                   = 401,
    eRtspPaymentRequired                = 402,
    eRtspForbidden                      = 403,
    eRtspNotFound                       = 404,
    eRtspMethodNotAllowed               = 405,
    eRtspNotAcceptable                  = 406,
    eRtspProxyAuthenticationRequired    = 407,
    eRtspRequestTimeout                 = 408,
    eRtspGone                           = 410,
    eRtspLengthRequired                 = 411,
    eRtspPreconditionFailed             = 412,
    eRtspRequestEntityTooLarge          = 413,
    eRtspRequestUriTooLarge             = 414,
    eRtspUnsupportedMediaType           = 415,
    eRtspParameterNotUnderstood         = 451,
    eRtspConferenceNotFound             = 452,
    eRtspNotEnoughBandwidth             = 453,
    eRtspSessionNotFound                = 454,
    eRtspMethodNotValidInThisState      = 455,
    eRtspHeaderFieldNotValidForResource = 456,
    eRtspInvalidRange                   = 457,
    eRtspParameterIsReadOnly            = 458,
    eRtspAggregateOperationNotAllowed   = 459,
    eRtspOnlyAggregateOperationAllowed  = 460,
    eRtspUnsupportedTransport           = 461,
    eRtspDestinationUnreachable         = 462,
    eRtspInternalServerError            = 500,
    eRtspNotImplemented                 = 501,
    eRtspBadGateway                     = 502,
    eRtspServiceUnavailable             = 503,
    eRtspGatewayTimeout                 = 504,
    eRtspRTSPVersionNotSupported        = 505,
    eRtspOptionNotSupported             = 551
};

class CRtspCode
{
	DECLARE_SINGLETON(CRtspCode)

private:
	CRtspCode();
	~CRtspCode();

public:
	const std::string& GetRtspCodeDesc(ERtspCode eCode) const;

private:
	std::map<ERtspCode, std::string>    m_mapRtspCodeDesc;
    std::string                         m_strDummy;
};

#endif
