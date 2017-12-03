/*
 * UrlCodec.h
 *
 *  Created on: May 19, 2016
 *      Author: lory
 */

#ifndef ___URL__CODEC__20160519___
#define ___URL__CODEC__20160519___

#include "OSType.h"
#include <string>

class CUrlCodec
{
public:
	static std::string UrlEncode(const std::string &str);
	static std::string UrlDecode(const std::string &str);

protected:
	static Char ToHex(UInt8 x);
	static UInt8 FromHex(UInt8 x);
};

#endif
