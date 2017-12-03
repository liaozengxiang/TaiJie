/*
 * UrlCodec.cpp
 *
 *  Created on: May 19, 2016
 *      Author: lory
 */

#include "../Include/UrlCodec.h"

std::string CUrlCodec::UrlEncode(const std::string &str)
{
	size_t nLen = str.length();
	LPCSTR lpszStr = str.c_str();

	std::string strTemp;
	strTemp.reserve(nLen * 3);

	for (size_t i = 0; i < nLen; ++i)
	{
		if (isalnum((UInt8)lpszStr[i])
			|| (lpszStr[i] == '-')
			|| (lpszStr[i] == '_')
			|| (lpszStr[i] == '.')
			|| (lpszStr[i] == '~'))
		{
			// 0-9a-zA-Z-_.~ 不用编码
			strTemp += lpszStr[i];
		}
		else if (lpszStr[i] == ' ')
		{
			// 空格被替换为 +
			strTemp += "+";
		}
		else
		{
			// 其他字符转换为 2 位十六进制字符表示，并在之前加上 % 字符
			strTemp += '%';
			strTemp += ToHex((UInt8)lpszStr[i] >> 4);
			strTemp += ToHex((UInt8)lpszStr[i] & 0x0F);
		}
	}

	return strTemp;
}

std::string CUrlCodec::UrlDecode(const std::string &str)
{
	size_t nLen = str.length();
	LPCSTR lpszStr = str.c_str();

	std::string strTemp;
	strTemp.reserve(nLen);

	for (size_t i = 0; i < nLen; ++i)
	{
		if (lpszStr[i] == '+')
		{
			strTemp += ' ';
		}
		else if (lpszStr[i] == '%')
		{
			if (i + 2 >= nLen)
			{
				// 百分号后面必须携带二位十六进制字符
				strTemp.clear();
				break;
			}
			
			UInt8 high = FromHex((UInt8)lpszStr[++i]);
			UInt8 low  = FromHex((UInt8)lpszStr[++i]);
			if (high > 0x0F || low > 0x0F)
			{
				// 十六进制数必须在 0-15 之间，非法的值表示解析失败
				strTemp.clear();
				break;
			}

			strTemp += (Char)(high * 16 + low);
		}
		else
		{
			strTemp += lpszStr[i];
		}
	}

	return strTemp;
}

Char CUrlCodec::ToHex(UInt8 x)
{
	// x 的取值必为 0-15, 0-9 被加上 '0' 字符，10-15 被加上 'A' 字符
	return (x > 9 ? x + 55 : x + 48);
}

/*
 * 功能: 通过十六进制字符返回 10 进制数
 * 返回: 成功时返回 0-15 之间的整数，失败时返回值大于 15
 */
UInt8 CUrlCodec::FromHex(UInt8 x)
{
	UInt8 y = 0xFF;

	if (x >= 'A' && x <= 'Z')
	{
		y = x - 'A' + 10;
	}
	else if (x >= 'a' && x <= 'z')
	{
		y = x - 'a' + 10;
	}
	else if (x >= '0' && x <= '9')
	{
		y = x - '0';
	}

	return y;
}
