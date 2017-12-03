#include "../Include/Base64.h"

std::string CBase64::Encode(const UInt8 *pBuffer, Int32 nLen)
{
	// 编码表
	static LPCSTR lpszEncodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	// Base64 编码后的长度约比原串增加 33% 的长度，预先分配缓冲区
	std::string strEncode;
	strEncode.reserve((size_t)(nLen * 4.0f / 3));

	unsigned char byTemp[4];
	int nLineLen=0;
	
	Int32 nLoopCount = nLen / 3;
	for (Int32 i = 0; i < nLoopCount; ++i)
	{
		// 对源串的每 3 个字节，使用 4 个 ASCII 码进行编码
		byTemp[1] = *pBuffer++;
		byTemp[2] = *pBuffer++;
		byTemp[3] = *pBuffer++;

		strEncode += lpszEncodeTable[byTemp[1] >> 2];
		strEncode += lpszEncodeTable[((byTemp[1] << 4) | (byTemp[2] >> 4)) & 0x3F];
		strEncode += lpszEncodeTable[((byTemp[2] << 2) | (byTemp[3] >> 6)) & 0x3F];
		strEncode += lpszEncodeTable[byTemp[3] & 0x3F];

		// 若一行的长度达到 76 行时，必须添加 \r\n
		nLineLen += 4;
		if (nLineLen == 76)
		{
			strEncode += "\r\n";
			nLineLen = 0;
		}
	}

	// 上述循环只针对前面 3 的整数倍数据进行编码，下面需要对剩余数据进行编码（只可能剩余 0/1/2 位的可能性）
	Int32 nLeftLen = nLen % 3;
	if (nLeftLen == 1)
	{
		byTemp[0] = *pBuffer++;

		strEncode += lpszEncodeTable[(byTemp[0] & 0xFC) >> 2];
		strEncode += lpszEncodeTable[((byTemp[0] & 0x03) << 4)];

		// 不足的位使用 = 补充
		strEncode += "==";
	}
	else if (nLeftLen == 2)
	{
		byTemp[0] = *pBuffer++;
		byTemp[1] = *pBuffer++;
		
		strEncode += lpszEncodeTable[(byTemp[0] & 0xFC) >> 2];
		strEncode += lpszEncodeTable[((byTemp[0] & 0x03) << 4) | ((byTemp[1] & 0xF0) >> 4)];
		strEncode += lpszEncodeTable[((byTemp[1] & 0x0F) << 2)];

		// 不足的位使用 = 补充
		strEncode += "=";
	}

	return strEncode;
}

std::string CBase64::Decode(LPCSTR pBuffer, Int32 nLen)
{
	const UInt8 *byBuffer = (const UInt8 *)pBuffer;

	// 解码表（为 0 的值是为了索引方便而填充的无效数据）
	static Int32 nDecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};

	std::string strDecode;
	strDecode.reserve((size_t)(nLen * 3.0f / 4));

	Int32 nValue;
	Int32 i = 0;
	while (i < nLen)
	{
		if (*byBuffer == '\r')
		{
			// 跳过回车换行
			byBuffer += 2;
			i += 2;
		}
		else
		{
			nValue = nDecodeTable[*byBuffer++] << 18;
			nValue += nDecodeTable[*byBuffer++] << 12;

			strDecode += (nValue & 0x00FF0000) >> 16;
			if (*pBuffer != '=')
			{
				nValue += nDecodeTable[*byBuffer++] << 6;
				strDecode += (nValue & 0x0000FF00) >> 8;

				if (*byBuffer != '=')
				{
					nValue += nDecodeTable[*byBuffer++];
					strDecode +=nValue & 0x000000FF;
				}
			}

			i += 4;
		}
	}

	return strDecode;
}
