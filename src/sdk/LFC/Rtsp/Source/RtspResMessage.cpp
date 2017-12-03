#include "../Include/RtspResMessage.h"
#include <sstream>
#include <list>
#include <stdlib.h>

CRtspResMessage::CRtspResMessage()
{
}

CRtspResMessage::~CRtspResMessage()
{
}

Bool CRtspResMessage::SetMessage(const std::string &strHeader, const std::string &strBody)
{
	Bool bSuccess = False;

	do
	{
		m_nCode = 0;
		m_strCodeDesc.clear();
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

		m_strProtocol = reqLine.front();
		reqLine.pop_front();

		m_nCode = atoi(reqLine.front().c_str());
		reqLine.pop_front();

		m_strCodeDesc = reqLine.front();
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

std::string CRtspResMessage::GetHeader() const
{
	std::ostringstream oss;
	oss << m_strProtocol << " " << m_nCode << " " << m_strCodeDesc << "\r\n"
		<< CRtspMessage::GetHeader();

	return oss.str();
}

std::string CRtspResMessage::GetPacket() const
{
	std::ostringstream oss;
	oss << m_strProtocol << " " << m_nCode << " " << m_strCodeDesc << "\r\n"
		<< CRtspMessage::GetHeader()
		<< "\r\n" << m_strBody;

	return oss.str();
}

void CRtspResMessage::SetCode(Int32 nCode, const std::string &strDesc, const std::string &strProtocol)
{
	m_nCode = nCode;
	m_strCodeDesc = strDesc;
	m_strProtocol = strProtocol;
}

Int32 CRtspResMessage::GetCode() const
{
	return m_nCode;
}

std::string CRtspResMessage::GetCodeDesc() const
{
	return m_strCodeDesc;
}

std::string CRtspResMessage::GetProtocol() const
{
	return m_strProtocol;
}
