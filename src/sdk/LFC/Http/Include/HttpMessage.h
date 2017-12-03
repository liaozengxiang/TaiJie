#ifndef ___HTTP__MESSAGE__20150512___
#define ___HTTP__MESSAGE__20150512___

#include <Common/Common.h>
#include <map>
#include <string>

// Http 消息的键比较函数，键不区分大小写
struct SHttpMsgKeyCompareFunc
{
	bool operator() (const std::string &a, const std::string &b) const
	{
		return (STR::InsensitiveCompare(a.c_str(), b.c_str()) < 0);
	}
};

class CHttpMessage
{
public:
	CHttpMessage();
	virtual ~CHttpMessage();

public:
	virtual Bool IsRequest() const = 0;
	virtual std::string GetHeader() const;
	virtual std::string GetPacket() const = 0;
	
	Bool DoesHeaderFieldExist(const std::string &strField) const;
	std::string GetHeaderField(const std::string &strField) const;
	std::string GetBody() const;

	void RemoveHeaderField(const std::string &strField);
	void SetHeaderField(const std::string &strField, const std::string &strValue);
	void SetBody(const std::string &strBody, const std::string &strContentType);

protected:
	std::map<std::string, std::string, SHttpMsgKeyCompareFunc>		m_mapHeaders;
	std::string														m_strBody;
};

#endif
