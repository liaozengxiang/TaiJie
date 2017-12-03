#ifndef ___BASE64__20150205___
#define ___BASE64__20150205___

#include "OSType.h"
#include <string>

class CBase64
{
public:
	static std::string Encode(const UInt8 *pBuffer, Int32 nLen);
	static std::string Decode(LPCSTR pBuffer, Int32 nLen);
};

#endif
