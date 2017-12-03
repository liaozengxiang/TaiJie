#include "../Include/RtspMessageParser.h"

CRtspMessageParser::CRtspMessageParser(IRtspMessageParserEvent *pEvent)
{
	Assert(pEvent != NULL);
	m_pEvent = pEvent;
	m_nBodySize = 0;
	m_eStatus = STATUS_PARSING_HEADER;
}

CRtspMessageParser::~CRtspMessageParser()
{
}

Bool CRtspMessageParser::Parse(LPCSTR lpszBuff, Int32 nLen)
{
	Bool bSuccess = True;
	Int32 nParsedSize = 0;

	while (nParsedSize < nLen)
	{
		if (STATUS_PARSING_HEADER == m_eStatus)
		{
			Int32 nResult = ParseHeader(lpszBuff + nParsedSize, nLen - nParsedSize);
			if (-1 == nResult)
			{
				bSuccess = False;
				break;
			}

			Assert(nResult > 0);
			nParsedSize += nResult;
		}
		else if (STATUS_PARSING_BODY == m_eStatus)
		{
			Int32 nResult = ParseBody(lpszBuff + nParsedSize, nLen - nParsedSize);
			if (-1 == nResult)
			{
				bSuccess = False;
				break;
			}

			Assert(nResult > 0);
			nParsedSize += nResult;
		}
	}

	return bSuccess;
}

Int32 CRtspMessageParser::ParseHeader(LPCSTR lpszBuff, Int32 nLen)
{
	Assert(m_eStatus == STATUS_PARSING_HEADER);

	Int32 nOriginSize = m_strHeader.length();
	m_strHeader.append(lpszBuff, nLen);
	std::string::size_type nPos = m_strHeader.find("\r\n\r\n");
	if (nPos == std::string::npos)
	{
		return nLen;
	}

	// 计算头的长度
	Int32 nHeaderSize = nPos + 4;

	// 去掉头中多余收到的数据
	Int32 nExtraSize = m_strHeader.length() - nHeaderSize;
	if (nExtraSize > 0)
	{
		m_strHeader.erase(nHeaderSize);
	}

	// 查询是否有 Body
	std::string strHeader = m_strHeader;
	STR::ToLower(strHeader);
	nPos = strHeader.find("content-length:");
	if (nPos == std::string::npos)
	{
		// 该包无 Body, 向上层回调
		CommitPacket(m_strHeader, "");
		m_strHeader.clear();
	}
	else
	{
		// 该包带有 Body, 解析出 Body 的长度
		nPos += 15;		// strlen("Content-Length:")
		if (1 != sscanf(m_strHeader.c_str() + nPos, "%d", &m_nBodySize))
		{
			// 解析 Body 出错
			return -1;
		}

		if (m_nBodySize < 0)
		{
			return -1;
		}
		else if (m_nBodySize == 0)
		{
			// 该包无 Body, 向上层回调
			CommitPacket(m_strHeader, "");
			m_strHeader.clear();
		}
		else
		{
			m_eStatus = STATUS_PARSING_BODY;
		}
	}

	return nHeaderSize - nOriginSize;
}

Int32 CRtspMessageParser::ParseBody(LPCSTR lpszBuff, Int32 nLen)
{
	Assert(m_eStatus == STATUS_PARSING_BODY);

	Int32 nReadSize = (nLen > m_nBodySize) ? m_nBodySize : nLen;
	m_strBody.append(lpszBuff, nReadSize);
	m_nBodySize -= nReadSize;

	if (m_nBodySize == 0)
	{
		// 整个 Body 已收完，向上层回调
		CommitPacket(m_strHeader, m_strBody);
		m_strHeader.clear();
		m_strBody.clear();

		m_eStatus = STATUS_PARSING_HEADER;
	}

	return nReadSize;
}

void CRtspMessageParser::CommitPacket(const std::string &strHeader, const std::string &strBody) const
{
	Assert(m_pEvent != NULL);
	m_pEvent->OnParseRtspPacket(strHeader, strBody);
}
