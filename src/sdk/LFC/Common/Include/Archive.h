/*
 * Copyright (C) 2014-2014 All rights reserved.
 * Powered by Lory Liao, on May 10th, 2014
 *
 * CArchive 类用于实现对象的串行化操作
 * 它支持基本数据类型和大部份 stl 的数据类型
 * 对于用户自定义类型，用户只需实现 ISerialize 接口便可同样支持串行化操作
 */

#ifndef ___ARCHIVE__H__20140510___
#define ___ARCHIVE__H__20140510___

#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>
#include "OSType.h"

class CArchive;
struct ISerialize
{
	virtual void OnStore(CArchive &arch) const                         = 0;				// 在对象被序列化时被调用
	virtual void OnLoad(CArchive &arch)                                = 0;				// 在对象被反序列化时被调用
	virtual ~ISerialize(){}
};

class CArchive
{
public:
	CArchive(std::ifstream *lpFile);
	CArchive(std::ofstream *lpFile);
	virtual ~CArchive();

	// 隐式类型转换，用于判断一个插入或提取操作是否成功
	operator bool()	const { return m_bSuccess; }

	// 插入操作
	virtual CArchive& operator <<(const ISerialize &obj);
	virtual CArchive& operator <<(Char c);
	virtual CArchive& operator <<(Int16 nValue);
	virtual CArchive& operator <<(Int32 nValue);
	virtual CArchive& operator <<(Int64 lValue);
	virtual CArchive& operator <<(UInt8 byValue);
	virtual CArchive& operator <<(UInt16 wValue);
	virtual CArchive& operator <<(UInt32 dwValue);
	virtual CArchive& operator <<(UInt64 qwValue);
	virtual CArchive& operator <<(LPCSTR lpszStr);
	virtual CArchive& operator <<(const std::string &str);

	template <typename T>
	CArchive& operator <<(const std::list<T> &lst)
	{
		Int32 nLen = lst.size();
		WriteSize(nLen);
		
		typename std::list<T>::const_iterator it = lst.begin();
		for (; it != lst.end(); ++ it)
		{
			operator <<(*it);

			if (!m_bSuccess)
			{
				break;
			}
		}
		
		return *this;
	}

	template <typename T>
	CArchive& operator <<(const std::vector<T> &vec)
	{
		Int32 nLen = vec.size();
		WriteSize(nLen);
		
		typename std::vector<T>::const_iterator it = vec.begin();
		for (; it != vec.end(); ++ it)
		{
			operator <<(*it);

			if (!m_bSuccess)
			{
				break;
			}
		}
		
		return *this;
	}

	template <typename T>
	CArchive& operator <<(const std::set<T> &st)
	{
		Int32 nLen = st.count();
		WriteSize(nLen);
		
		typename std::set<T>::const_iterator it = st.begin();
		for (; it != st.end(); ++ it)
		{
			operator <<(*it);

			if (!m_bSuccess)
			{
				break;
			}
		}
		
		return *this;
	}

	template <typename K, typename V>
	CArchive& operator <<(const std::map<K, V> &mp)
	{
		Int32 nLen = mp.size();
		WriteSize(nLen);
		
		typename std::map<K, V>::const_iterator it = mp.begin();
		for (; it != mp.end(); ++ it)
		{
			*this << it->first << it->second;

			if (!m_bSuccess)
			{
				break;
			}
		}
		
		return *this;
	}

	// 提取操作
	virtual CArchive& operator >>(ISerialize &obj);
	virtual CArchive& operator >>(Char &c);
	virtual CArchive& operator >>(Int16 &nValue);
	virtual CArchive& operator >>(Int32 &nValue);
	virtual CArchive& operator >>(Int64 &lValue);
	virtual CArchive& operator >>(UInt8 &byValue);
	virtual CArchive& operator >>(UInt16 &wValue);
	virtual CArchive& operator >>(UInt32 &dwValue);
	virtual CArchive& operator >>(UInt64 &qwValue);
	virtual CArchive& operator >>(LPSTR szBuff);
	virtual CArchive& operator >>(std::string &str);

	template <typename T>
	CArchive& operator >>(std::list<T> &lst)
	{
		Int32 nLen = ReadSize();
		while (nLen --)
		{
			T value;
			operator >>(value);

			if (!m_bSuccess)
			{
				break;
			}

			lst.push_back(value);
		}

		return *this;
	}
	
	template <typename T>
	CArchive& operator >>(std::vector<T> &vec)
	{
		Int32 nLen = ReadSize();
		vec.reserve(nLen);

		while (nLen --)
		{
			T value;
			operator >>(value);

			if (!m_bSuccess)
			{
				break;
			}

			vec.push_back(value);
		}

		return *this;
	}
	
	template <typename T>
	CArchive& operator >>(std::set<T> &st)
	{
		Int32 nLen = ReadSize();
		while (nLen --)
		{
			T value;
			operator >>(value);

			if (!m_bSuccess)
			{
				break;
			}

			st.insert(value);
		}

		return *this;
	}
	
	template <typename K, typename V>
	CArchive& operator >>(std::map<K, V> &mp)
	{
		Int32 nLen = ReadSize();
		while (nLen --)
		{
			K key;
			V value;

			*this >> key >> value;

			if (!m_bSuccess)
			{
				break;
			}

			mp[key] = value;
		}

		return *this;
	}

protected:
	Bool  ReadFile(void *buff, Int32 nSize);
	Bool  WriteFile(const void *buff, Int32 nSize);

	virtual Bool  WriteSize(Int32 nSize);						// 序列化一个长度
	virtual Int32 ReadSize();									// 反序列化一个长度

protected:
	std::ifstream*					m_lpFileRead;
	std::ofstream*					m_lpFileWrite;
	bool							m_bSuccess;
};

#endif
