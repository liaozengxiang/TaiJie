#ifndef ___HTTP__RES__MESSAGE__20150512___
#define ___HTTP__RES__MESSAGE__20150512___

#include "HttpMessage.h"

class CHttpResMessage : public CHttpMessage
{
public:
	CHttpResMessage();
	virtual ~CHttpResMessage();

public:
	Bool SetMessage(const std::string &strHeader, const std::string &strBody);
	virtual Bool IsRequest() const { return False; }
	virtual std::string GetHeader() const;
	virtual std::string GetPacket() const;

	void SetCode(Int32 nCode, const std::string &strDesc, const std::string &strProtocol = "HTTP/1.1");
	Int32 GetCode() const;
	
	std::string GetCodeDesc() const;
	std::string GetProtocol() const;

private:
	Int32									m_nCode;
	std::string								m_strCodeDesc;
	std::string								m_strProtocol;
};

#endif
