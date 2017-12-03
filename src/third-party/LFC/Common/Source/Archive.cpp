#include <string.h>
#include "../Include/Archive.h"

CArchive::CArchive(std::ifstream *lpFile)
{
	m_lpFileRead = lpFile;
	m_lpFileWrite = NULL;
}

CArchive::CArchive(std::ofstream *lpFile)
{
	m_lpFileRead = NULL;
	m_lpFileWrite = lpFile;
}

CArchive::~CArchive()
{
}

/************************************************************************/
/* 插入操作                                                             */
/************************************************************************/

CArchive& CArchive::operator <<(const ISerialize &obj)
{
	obj.OnStore(*this);
	return *this;
}

CArchive& CArchive::operator <<(Char c)
{
	WriteFile(&c, sizeof(Char));
	return *this;
}

CArchive& CArchive::operator <<(Int16 nValue)
{
	WriteFile(&nValue, sizeof(Int16));
	return *this;
}

CArchive& CArchive::operator <<(Int32 nValue)
{
	WriteFile(&nValue, sizeof(Int32));
	return *this;
}

CArchive& CArchive::operator <<(Int64 lValue)
{
	WriteFile(&lValue, sizeof(Int64));
	return *this;
}

CArchive& CArchive::operator <<(UInt8 byValue)
{
	WriteFile(&byValue, sizeof(UInt8));
	return *this;
}

CArchive& CArchive::operator <<(UInt16 wValue)
{
	WriteFile(&wValue, sizeof(UInt16));
	return *this;
}

CArchive& CArchive::operator <<(UInt32 dwValue)
{
	WriteFile(&dwValue, sizeof(UInt32));
	return *this;
}

CArchive& CArchive::operator <<(UInt64 qwValue)
{
	WriteFile(&qwValue, sizeof(UInt64));
	return *this;
}

CArchive& CArchive::operator <<(LPCSTR lpszStr)
{
	Int32 nLen = strlen(lpszStr);
	
	if (WriteSize(nLen))
	{
		WriteFile(lpszStr, nLen);
	}

	return *this;
}

CArchive& CArchive::operator <<(const std::string &str)
{
	Int32 nLen = str.length();

	if (WriteSize(nLen))
	{
		WriteFile(str.c_str(), nLen);
	}

	return *this;
}

/************************************************************************/
/* 提取操作                                                             */
/************************************************************************/

CArchive& CArchive::operator >>(ISerialize &obj)
{
	obj.OnLoad(*this);
	return *this;
}

CArchive& CArchive::operator >>(Char &c)
{
	ReadFile(&c, sizeof(Char));
	return *this;
}

CArchive& CArchive::operator >>(Int16 &nValue)
{
	ReadFile(&nValue, sizeof(Int16));
	return *this;
}

CArchive& CArchive::operator >>(Int32 &nValue)
{
	ReadFile(&nValue, sizeof(Int32));
	return *this;
}

CArchive& CArchive::operator >>(Int64 &lValue)
{
	ReadFile(&lValue, sizeof(Int64));
	return *this;
}

CArchive& CArchive::operator >>(UInt8 &byValue)
{
	ReadFile(&byValue, sizeof(UInt8));
	return *this;
}

CArchive& CArchive::operator >>(UInt16 &wValue)
{
	ReadFile(&wValue, sizeof(UInt16));
	return *this;
}

CArchive& CArchive::operator >>(UInt32 &dwValue)
{
	ReadFile(&dwValue, sizeof(UInt32));
	return *this;
}

CArchive& CArchive::operator >>(UInt64 &qwValue)
{
	ReadFile(&qwValue, sizeof(UInt64));
	return *this;
}

CArchive& CArchive::operator >>(LPSTR szBuff)
{
	Int32 nLen = ReadSize();
	if (m_bSuccess)
	{
		ReadFile(szBuff, nLen);
		szBuff[nLen] = '\0';
	}

	return *this;
}

CArchive& CArchive::operator >>(std::string &str)
{
	Int32 nLen = ReadSize();
	if (m_bSuccess)
	{
		str.resize(nLen + 1);
		str[nLen] = '\0';
		ReadFile(const_cast<char *>(str.c_str()), nLen);
	}

	return *this;
}

/************************************************************************/
/* 辅助函数                                                             */
/************************************************************************/

Bool CArchive::WriteFile(const void *buff, Int32 nSize)
{
	// ofstream::write 返回的是 ofstream 对象的引用，根据隐式类型转换可以返回 bool 值
	m_bSuccess = m_lpFileWrite->write((const char *)buff, nSize);
	return m_bSuccess;
}

Bool CArchive::ReadFile(void *buff, Int32 nSize)
{
	// ifstream::read 返回的是 ifstream 对象的引用，根据隐式类型转换可以返回 bool 值
	m_bSuccess = m_lpFileRead->read((char *)buff, nSize);
	return m_bSuccess;
}

Bool CArchive::WriteSize(Int32 nSize)
{
	UInt8 nBytes;
	if (nSize <= 0xFF)
	{
		nBytes = 1;
	}
	else if (nSize < 0xFFFF)
	{
		nBytes = 2;
	}
	else if (nSize < 0xFFFFFF)
	{
		nBytes = 3;
	}
	else
	{
		nBytes = 4;
	}
	
	if (!WriteFile(&nBytes, 1))
	{
		return False;
	}

	if (!WriteFile(&nSize, nBytes))
	{
		return False;
	}

	return True;
}

Int32 CArchive::ReadSize()
{
	UInt8 nBytes;
	Int32 nSize = 0;

	if (!ReadFile(&nBytes, 1))
	{
		return 0;
	}

	if (!ReadFile(&nSize, nBytes))
	{
		return 0;
	}

	return nSize;
}
