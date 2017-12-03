#ifndef ___STR__20140720___
#define ___STR__20140720___

#include "OSType.h"
#include <string>
#include <list>

namespace STR
{
    // 字符串拆分与合并
	void        Explode(const char *lpszSrc, const char *lpszDelimiter, std::list<std::string> &subList);
	std::string Implode(const std::list<std::string> &lines, const char *lpszDelimiter);

    // 字符串转换为数字
	UInt64 Str2UInt64(LPCSTR lpszNum);
	Int64  Str2Int64(LPCSTR lpszNum);
	UInt32 Str2UInt32(LPCSTR lpszNum);
	Int32  Str2Int32(LPCSTR lpszNum);

    // 数字转换为字符串
    std::string NumberToString(Int32 nValue);
    std::string NumberToString(UInt32 uValue);
    std::string NumberToString(Int64 lValue);
    std::string NumberToString(UInt64 ulValue);

    // 字符串比较
	Int32 InsensitiveCompare(const char *s1, const char *s2);

    // 字符串大小写转换
	void ToUpper(std::string &str);
	void ToLower(std::string &str);

    // @strKeyItem: "AAA\\BBB\\CCC"
    // @strEndKey:  "A|B" (end as "A" or "B")
    std::string GetKeyValue(const std::string &strSrc, const std::string &strKeyItem, const std::string &strEndKey);
    std::string GetKeyValue(const std::string &strSrc, const std::string &strKeyItem, const std::string &strEndKey, Int32 &nStartPos);
    std::string GetKeyValue(const std::string &strSrc, const std::string &strKeyItem, Int32 nLength);
}

#endif
