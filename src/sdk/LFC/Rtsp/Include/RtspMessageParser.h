#ifndef ___RTSP__MESSAGE__PARSER__20160519___
#define ___RTSP__MESSAGE__PARSER__20160519___

#include <Common/Common.h>
#include <string>

struct IRtspMessageParserEvent
{
	virtual ~IRtspMessageParserEvent() {}
	virtual void OnParseRtspPacket(const std::string &strHeader, const std::string &strBody) = 0;
};

class CRtspMessageParser
{
public:
	CRtspMessageParser(IRtspMessageParserEvent *pEvent);
	~CRtspMessageParser();

public:
	Bool Parse(LPCSTR lpszBuff, Int32 nLen);

private:
	Int32 ParseHeader(LPCSTR lpszBuff, Int32 nLen);
	Int32 ParseBody(LPCSTR lpszBuff, Int32 nLen);
	void  CommitPacket(const std::string &strHeader, const std::string &strBody) const;

private:
	enum
	{
		STATUS_PARSING_HEADER,
		STATUS_PARSING_BODY
	};

	IRtspMessageParserEvent*	m_pEvent;
	std::string				    m_strHeader;
	Int32					    m_nBodySize;
	std::string				    m_strBody;
	Int32				    	m_eStatus;
};

#endif
