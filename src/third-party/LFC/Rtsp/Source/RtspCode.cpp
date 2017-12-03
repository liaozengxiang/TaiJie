#include "../Include/RtspCode.h"


CRtspCode::CRtspCode()
{
    m_strDummy = "Unknown";

    m_mapRtspCodeDesc[eRtspContinue                      ] = "Continue";                                // 继续
    m_mapRtspCodeDesc[eRtspOK                            ] = "OK";                                      // 成功
    m_mapRtspCodeDesc[eRtspCreated                       ] = "Created";
    m_mapRtspCodeDesc[eRtspLowOnStorageSpace             ] = "Low on Storage Space";                    // 存储空间低
    m_mapRtspCodeDesc[eRtspMultipleChoices               ] = "Multiple Choices";                        // 多种选择
    m_mapRtspCodeDesc[eRtspMovedPermanently              ] = "Moved Permanently";                       // 永久移动
    m_mapRtspCodeDesc[eRtspMovedTemporarily              ] = "Moved Temporarily";                       // 临时移动
    m_mapRtspCodeDesc[eRtspSeeOther                      ] = "See Other";
    m_mapRtspCodeDesc[eRtspNotModified                   ] = "Not Modified";                            // 未修改
    m_mapRtspCodeDesc[eRtspUseProxy                      ] = "Use Proxy";                               // 使用代理
    m_mapRtspCodeDesc[eRtspBadRequest                    ] = "Bad Request";                             // 请求出错
    m_mapRtspCodeDesc[eRtspUnauthorized                  ] = "Unauthorized";                            // 未授权
    m_mapRtspCodeDesc[eRtspPaymentRequired               ] = "Payment Required";                        // 需要付款
    m_mapRtspCodeDesc[eRtspForbidden                     ] = "Forbidden";                               // 拒绝
    m_mapRtspCodeDesc[eRtspNotFound                      ] = "Not Found";                               // 未找到
    m_mapRtspCodeDesc[eRtspMethodNotAllowed              ] = "Method Not Allowed";                      // 方法不允许
    m_mapRtspCodeDesc[eRtspNotAcceptable                 ] = "Not Acceptable";                          // 不可接受
    m_mapRtspCodeDesc[eRtspProxyAuthenticationRequired   ] = "Proxy Authentication Required";           // 需要代理授权
    m_mapRtspCodeDesc[eRtspRequestTimeout                ] = "Request Time-out";                        // 请求超时
    m_mapRtspCodeDesc[eRtspGone                          ] = "Gone";
    m_mapRtspCodeDesc[eRtspLengthRequired                ] = "Length Required";                         // 需要长度
    m_mapRtspCodeDesc[eRtspPreconditionFailed            ] = "Precondition Failed";                     // 先决条件失败
    m_mapRtspCodeDesc[eRtspRequestEntityTooLarge         ] = "Request Entity Too Large";                // 请求实体过大
    m_mapRtspCodeDesc[eRtspRequestUriTooLarge            ] = "Request-URI Too Large";                   // 请求URI过长
    m_mapRtspCodeDesc[eRtspUnsupportedMediaType          ] = "Unsupported Media Type";                  // 媒体类型不支持
    m_mapRtspCodeDesc[eRtspParameterNotUnderstood        ] = "Parameter Not Understood";                // 参数不能理解
    m_mapRtspCodeDesc[eRtspConferenceNotFound            ] = "Conference Not Found";                    // 会议未找到
    m_mapRtspCodeDesc[eRtspNotEnoughBandwidth            ] = "Not Enough Bandwidth";                    // 带宽不足
    m_mapRtspCodeDesc[eRtspSessionNotFound               ] = "Session Not Found";                       // 会话未找到
    m_mapRtspCodeDesc[eRtspMethodNotValidInThisState     ] = "Method Not Valid in This State";          // 本状态下该方法无效
    m_mapRtspCodeDesc[eRtspHeaderFieldNotValidForResource] = "Header Field Not Valid for Resource";     // 标题域对资源无效
    m_mapRtspCodeDesc[eRtspInvalidRange                  ] = "Invalid Range";                           // 无效范围
    m_mapRtspCodeDesc[eRtspParameterIsReadOnly           ] = "Parameter Is Read-Only";                  // 参数只读
    m_mapRtspCodeDesc[eRtspAggregateOperationNotAllowed  ] = "Aggregate operation not allowed";         // 不允许合操作
    m_mapRtspCodeDesc[eRtspOnlyAggregateOperationAllowed ] = "Only aggregate operation allowed";        // 只允许合操作
    m_mapRtspCodeDesc[eRtspUnsupportedTransport          ] = "Unsupported transport";                   // 不支持的传输
    m_mapRtspCodeDesc[eRtspDestinationUnreachable        ] = "Destination unreachable";                 // 目标不可达
    m_mapRtspCodeDesc[eRtspInternalServerError           ] = "Internal Server Error";                   // 网络服务错误
    m_mapRtspCodeDesc[eRtspNotImplemented                ] = "Not Implemented";                         // 未实现
    m_mapRtspCodeDesc[eRtspBadGateway                    ] = "Bad Gateway";                             // 网关错误
    m_mapRtspCodeDesc[eRtspServiceUnavailable            ] = "Service Unavailable";                     // 服务不存在
    m_mapRtspCodeDesc[eRtspGatewayTimeout                ] = "Gateway Time-out";                        // 网关超时
    m_mapRtspCodeDesc[eRtspRTSPVersionNotSupported       ] = "RTSP Version not supported";              // RTSP版本不支持
    m_mapRtspCodeDesc[eRtspOptionNotSupported            ] = "Option not supported";                    // 选择不支持
}

CRtspCode::~CRtspCode()
{
}

const std::string& CRtspCode::GetRtspCodeDesc(ERtspCode eCode) const
{
	std::map<ERtspCode, std::string>::const_iterator it = m_mapRtspCodeDesc.find(eCode);
	if (it != m_mapRtspCodeDesc.end())
	{
		return it->second;
	}

	return m_strDummy;
}
