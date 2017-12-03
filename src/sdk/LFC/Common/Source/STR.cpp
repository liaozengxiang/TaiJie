#include "../Include/STR.h"
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <ctype.h>

/*
 * 函数: Explode
 * 功能: 将源字符串以 lpszDelimiter 为分界拆解成若干个子串，保存在 subList 中
 * 参数: 
 *       [in] lpszSrc       源字符串
 *       [in] lpszDelimiter 分界串
 *       [out]subList       用于保存输出的子串列表
 * 返回: N/A
 */
void STR::Explode(const char *lpszSrc, const char *lpszDelimiter, std::list<std::string> &subList)
{
	int nDelimiterLen = strlen(lpszDelimiter);
	const char *lpszFind;
	
	while (*lpszSrc != '\0')
	{
		lpszFind = strstr(lpszSrc, lpszDelimiter);
		if (NULL == lpszFind)
		{
			subList.push_back(lpszSrc);
			break;
		}
		
		int nSubLen = lpszFind - lpszSrc;
		if (0 == nSubLen)
		{
			subList.push_back("");
		}
		else
		{
			std::string strText(lpszSrc, lpszFind - lpszSrc);
			subList.push_back(strText);
		}
		
		lpszSrc = lpszFind + nDelimiterLen;
	}
	
	if (*lpszSrc == '\0')
	{
		// 如果字符串是以分隔串结尾，则需要加上一个空串
		subList.push_back("");
	}
}

/*
 * 函数: Implode
 * 功能: 将源字符串列表以 lpszDelimiter 为分界组合成一个字符串并返回
 * 参数: 
 *       lines         源字符串列表
 *       lpszDelimiter 分界串
 * 返回: 组合后的字符串
 */
std::string STR::Implode(const std::list<std::string> &lines, const char *lpszDelimiter)
{
	std::ostringstream oss;
	int nMaxIndex = lines.size() - 1;
	
	std::list<std::string>::const_iterator it = lines.begin();
	int i = 0;
	for (; it != lines.end(); ++ it, ++ i)
	{
		if (i < nMaxIndex)
		{
			oss << *it << lpszDelimiter;
		}
		else
		{
			// 最后一行拼接时不加分隔串
			oss << *it;
		}
	}
	
	return oss.str();
}

UInt64 STR::Str2UInt64(LPCSTR lpszNum)
{
	UInt64 qwNum = 0;
#ifdef OS_32
	sscanf(lpszNum, "%llu", &qwNum);
#else
	sscanf(lpszNum, "%lu", &qwNum);
#endif

	return qwNum;
}

Int64 STR::Str2Int64(LPCSTR lpszNum)
{
	Int64 lNum = 0;
#ifdef OS_32
	sscanf(lpszNum, "%lld", &lNum);
#else
	sscanf(lpszNum, "%ld", &lNum);
#endif

	return lNum;
}

UInt32 STR::Str2UInt32(LPCSTR lpszNum)
{
	UInt32 dwNum = 0;
	sscanf(lpszNum, "%u", &dwNum);

	return dwNum;
}

Int32 STR::Str2Int32(LPCSTR lpszNum)
{
	Int32 nNum = 0;
	sscanf(lpszNum, "%d", &nNum);

	return nNum;
}

std::string STR::NumberToString(Int32 nValue)
{
    char szValue[16];
    sprintf(szValue, "%d", nValue);
    return szValue;
}

std::string STR::NumberToString(UInt32 uValue)
{
    char szValue[16];
    sprintf(szValue, "%u", uValue);
    return szValue;
}

std::string STR::NumberToString(Int64 lValue)
{
    char szValue[24];
#ifdef OS_64
    sprintf(szValue, "%ld", lValue);
#else
    sprintf(szValue, "%lld", lValue);
#endif
    return szValue;
}

std::string STR::NumberToString(UInt64 ulValue)
{
    char szValue[24];
#ifdef OS_64
    sprintf(szValue, "%lu", ulValue);
#else
    sprintf(szValue, "%llu", ulValue);
#endif
    return szValue;
}

Int32 STR::InsensitiveCompare(const char *s1, const char *s2)
{
	Int32 ch1, ch2;

	do
	{
		if (((ch1 = (unsigned char)(*(s1++))) >= 'A') && (ch1 <= 'Z'))
		{
			ch1 += 0x20;
		}

		if (((ch2 = (unsigned char)(*(s2++))) >= 'A') && (ch2 <= 'Z'))
		{
			ch2 += 0x20;
		}
	} while (ch1 && (ch1 == ch2));

	return (ch1 - ch2);
}

void STR::ToUpper(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}

void STR::ToLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
}

std::string STR::GetKeyValue(const std::string &strSrc, const std::string &strKeyItem, const std::string &strEndKey)
{
    Int32 nPos = 0;
    return GetKeyValue(strSrc, strKeyItem, strEndKey, nPos);
}

std::string STR::GetKeyValue(const std::string &strSrc, const std::string &strKeyItem, const std::string &strEndKey, Int32 &nStartPos)
{
    std::string strStd = strSrc;
    std::string strItems[10];

    Int32 pos1 = 0;
    Int32 pos2 = strKeyItem.find("\\", pos1);
    Int32 n = 0;
    for (n = 0; pos2 != -1; ++n)
    {
        strItems[n] = strKeyItem.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;
        pos2 = strKeyItem.find("\\", pos1);
    }
    strItems[n] = strKeyItem.substr(pos1, strKeyItem.length() - pos1);

    Int32 iBegin = 0;
    int j = 0;
    for (j = 0; j <= n; ++j)
    {
        iBegin = strStd.find(strItems[j], nStartPos + iBegin) + strItems[j].length();
        if (iBegin == -1 + (Int32)strItems[j].length())
        {
            return "";
        }
    }

    // 分离结束字符串集合
    std::string strEnds[10];
    pos1 = 0;
    pos2 = strEndKey.find("|", pos1);
    for (n = 0; pos2 != -1; ++n)
    {
        strEnds[n] = strEndKey.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;
        pos2 = strEndKey.find("|", pos1);
    }
    strEnds[n] = strEndKey.substr(pos1, strEndKey.length() - pos1);
    strEnds[++n] = "\r\n";	// 如果在行结束 "\r\n" 之前没有找到 strEndKey, 则以 "\r\n" 结束
    strEnds[++n] = "\n";	// 其次找 "\n"

    Int32 iEndPos = -1;
    for (j = 0; j <= n; ++j)
    {
        if (strEnds[j].empty())
        {
            iEndPos = strStd.length();
            break;
        }
        iEndPos = strStd.find(strEnds[j], iBegin);
        if (iEndPos >= 0)
        {
            break;
        }
    }

    if (iEndPos < 0)
    {
        iEndPos = strSrc.size();
    }

    nStartPos = iEndPos;
    return strSrc.substr(iBegin, iEndPos - iBegin);
}

std::string STR::GetKeyValue(const std::string &strSrc, const std::string &strKeyItem, Int32 nLength)
{
    std::string strStd = strSrc;
    std::string strItems[10];
    Int32 pos1 = 0;
    Int32 pos2 = strKeyItem.find("\\", pos1);
    Int32 n;
    for (n = 0; pos2 != -1; ++n)
    {
        strItems[n] = strKeyItem.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;
        pos2 = strKeyItem.find("\\", pos1);
    }
    strItems[n] = strKeyItem.substr(pos1, strKeyItem.length() - pos1);

    Int32 iBegin = 0;
    for (Int32 j = 0; j <= n; ++j)
    {
        iBegin = strStd.find(strItems[j], iBegin) + strItems[j].length();
        if (iBegin == -1 + (Int32)strItems[j].length())
        {
            return "";
        }
    }

    Int32 iEndPos = iBegin + nLength;
    return strSrc.substr(iBegin, iEndPos - iBegin);
}
