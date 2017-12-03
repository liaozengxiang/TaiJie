#include "../Include/RtspMessage.h"
#include <sstream>

CRtspMessage::CRtspMessage()
{
}

CRtspMessage::~CRtspMessage()
{
}

std::string CRtspMessage::GetHeader() const
{
	std::ostringstream oss;
	std::map<std::string, std::string>::const_iterator it = m_mapHeaders.begin();
	for (; it != m_mapHeaders.end(); ++ it)
	{
		oss << it->first << ": " << it->second << "\r\n";
	}

	return oss.str();
}

Bool CRtspMessage::DoesHeaderFieldExist(const std::string &strField) const
{
	return (m_mapHeaders.find(strField) != m_mapHeaders.end());
}

std::string CRtspMessage::GetHeaderField(const std::string &strField) const
{
	std::map<std::string, std::string>::const_iterator it = m_mapHeaders.find(strField);
	if (it != m_mapHeaders.end())
	{
		return it->second;
	}

	return "";
}

std::string CRtspMessage::GetBody() const
{
	return m_strBody;
}

void CRtspMessage::RemoveHeaderField(const std::string &strField)
{
	m_mapHeaders.erase(strField);
}

void CRtspMessage::SetHeaderField(const std::string &strField, const std::string &strValue)
{
	m_mapHeaders[strField] = strValue;
}

void CRtspMessage::SetBody(const std::string &strBody, const std::string &strContentType)
{
	m_strBody = strBody;
	m_mapHeaders["Content-Type"] = strContentType;

	char szLen[32];
	sprintf(szLen, "%ld", (Int64)strBody.length());
	m_mapHeaders["Content-Length"] = szLen;
}
