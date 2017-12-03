#include "../Include/RtspReqMessage.h"
#include <sstream>
#include <list>

CRtspReqMessage::CRtspReqMessage()
{
}

CRtspReqMessage::~CRtspReqMessage()
{
}

Bool CRtspReqMessage::SetMessage(const std::string &strHeader, const std::string &strBody)
{
	Bool bSuccess = False;

	do
	{
		m_strMethod.clear();
		m_strUri.clear();
		m_strProtocol.clear();
		m_mapHeaders.clear();

		std::list<std::string> lineList;
		STR::Explode(strHeader.c_str(), "\r\n", lineList);
		if (lineList.empty())
		{
			break;
		}

		// 取出请求行
		std::string strReqLine = lineList.front();
		lineList.pop_front();

		std::list<std::string> reqLine;
		STR::Explode(strReqLine.c_str(), " ", reqLine);
		if (reqLine.size() != 3)
		{
			break;
		}

		m_strMethod = reqLine.front();
		reqLine.pop_front();

		SetUrl(reqLine.front());
		reqLine.pop_front();

		m_strProtocol = reqLine.front();
		reqLine.pop_front();

		std::list<std::string>::const_iterator it = lineList.begin();
		for (; it != lineList.end(); ++ it)
		{
			const std::string &str = *it;
			std::string::size_type nPos = str.find(':');
			if (nPos != std::string::npos)
			{
				std::string strKey(str.c_str(), nPos);
				if (nPos + 1 < str.length() && str.at(nPos + 1) == ' ')
				{
					++nPos;
				}
				SetHeaderField(strKey, str.c_str() + nPos + 1);
			}
		}

		if (!strBody.empty())
		{
			if (!DoesHeaderFieldExist("Content-Type"))
			{
				break;
			}

			SetBody(strBody, GetHeaderField("Content-Type"));
		}

		bSuccess = True;
	} while (0);

	return bSuccess;
}

std::string CRtspReqMessage::GetHeader() const
{
	std::ostringstream oss;
	oss << m_strMethod << " " << m_strUrl << " " << m_strProtocol << "\r\n"
		<< CRtspMessage::GetHeader();

	return oss.str();
}

std::string CRtspReqMessage::GetPacket() const
{
	std::ostringstream oss;
	oss << m_strMethod << " " << m_strUrl << " " << m_strProtocol << "\r\n"
		<< CRtspMessage::GetHeader()
		<< "\r\n" << m_strBody;

	return oss.str();
}

void CRtspReqMessage::SetRequestLine(const std::string &strMethod, const std::string &strUrl, const std::string &strProtocol)
{
	SetUrl(strUrl);
	m_strMethod = strMethod;
	m_strProtocol = strProtocol;
}

std::string CRtspReqMessage::GetMethod() const
{
	return m_strMethod;
}

std::string CRtspReqMessage::GetUri() const
{
	return m_strUri;
}

std::string CRtspReqMessage::GetProtocol() const
{
	return m_strProtocol;
}

Bool CRtspReqMessage::DesGetParameterExist(const std::string &strParamter) const
{
	return (m_mapParameters.find(strParamter) != m_mapParameters.end());
}

std::string CRtspReqMessage::GetParameter(const std::string &strParamter) const
{
	std::map<std::string, std::string>::const_iterator it = m_mapParameters.find(strParamter);
	if (it != m_mapParameters.end())
	{
		return it->second;
	}

	return "";
}

void CRtspReqMessage::SetParameter(const std::string &strParamter, const std::string &strValue)
{
	m_mapParameters[strParamter] = strValue;
	ComposeUrl();
}

void CRtspReqMessage::RemoveParameter(const std::string &strParamter)
{
	m_mapParameters.erase(strParamter);
	ComposeUrl();
}

void CRtspReqMessage::SetUrl(const std::string &strUrl)
{
	do
	{
		m_strUri.clear();
		m_mapParameters.clear();

		// 保存完整的 Url 地址
		m_strUrl = strUrl;

		// 解析出 Uri 和 Parameters
		// 查找第一个 ?, 后续的字符串为 key1=value1&key2=value2...
		std::string::size_type nPos = strUrl.find('?');
		if (nPos == std::string::npos)
		{
			m_strUri = strUrl;
			break;
		}

		m_strUri.assign(strUrl.c_str(), nPos);
		++nPos;

		std::list<std::string> lineList;
		STR::Explode(strUrl.c_str() + nPos, "&", lineList);
		for (std::list<std::string>::const_iterator it = lineList.begin(); it != lineList.end(); ++it)
		{
			// 查找 '=' 符，取出 key/value 对
			std::string::size_type nSeprator = it->find('=');
			if (nSeprator == std::string::npos)
			{
				continue;
			}

			std::string strKey(it->c_str(), nSeprator);
			m_mapParameters[strKey] = it->c_str() + nSeprator + 1;
		}
	} while (0);
}

void CRtspReqMessage::ComposeUrl()
{
	std::ostringstream oss;
	for (std::map<std::string, std::string>::const_iterator it = m_mapParameters.begin(); it != m_mapParameters.end(); ++it)
	{
		oss << "&" << it->first << "=" << it->second;
	}

	m_strUrl = m_strUri + "?" + (oss.str().c_str() + 1);
}
