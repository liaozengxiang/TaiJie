/*
 * Buffer.cpp
 *
 *  Created on: Jun 01, 2016
 *      Author: lory
 */

#include "../Include/Buffer.h"
#include "../Include/Assert.h"
#include <string.h>

CBuffer::CBuffer()
{
    m_bOwnBuffer = False;
    m_szBuffer   = NULL;
    m_nWriteLen  = 0;
    m_nReadLen   = 0;
    m_nCapacity  = 0;
}

CBuffer::~CBuffer()
{
    Clear();

    if (m_bOwnBuffer)
    {
        delete []m_szBuffer;
    }
    m_szBuffer = NULL;
}

Bool CBuffer::Initialize(Int32 nSize)
{
    Assert(nSize > 0);
    Assert(m_szBuffer == NULL && m_nCapacity == 0);
    Assert(m_nWriteLen == 0 && m_nReadLen == 0);

    m_bOwnBuffer = True;
    m_szBuffer = new Char[nSize];
    if (NULL == m_szBuffer)
    {
        return False;
    }

    m_nCapacity = nSize;
    return True;
}

Bool CBuffer::Initialize(LPSTR szBuffer, Int32 nSize)
{
    Assert(szBuffer != NULL);
    Assert(nSize > 0);

    Assert(m_szBuffer == NULL && m_nCapacity == 0);
    Assert(m_nWriteLen == 0 && m_nReadLen == 0);

    m_bOwnBuffer = False;
    m_szBuffer = szBuffer;
    m_nCapacity = nSize;

    return True;
}

void CBuffer::Clear()
{
    m_nWriteLen = 0;
    m_nReadLen  = 0;
}

Int32 CBuffer::Write(LPCSTR lpszBuffer, Int32 nLen)
{
    Assert(nLen > 0);

    Int32 nLeftSize = m_nCapacity - m_nWriteLen;
    if (nLen > nLeftSize)
    {
        nLen = nLeftSize;
    }

    if (nLen > 0)
    {
        memcpy(m_szBuffer + m_nWriteLen, lpszBuffer, nLen);
        m_nWriteLen += nLen;
    }

    return nLen;
}

Int32 CBuffer::Write(Int32 nLen)
{
    Assert(nLen > 0);

    Int32 nLeftSize = m_nCapacity - m_nWriteLen;
    if (nLen > nLeftSize)
    {
        nLen = nLeftSize;
    }

    m_nWriteLen += nLen;
    return nLen;
}

Int32 CBuffer::GetReadLen() const
{
    return m_nReadLen;
}

Int32 CBuffer::GetWriteLen() const
{
    return m_nWriteLen;
}

Int32 CBuffer::GetSpace() const
{
    return m_nCapacity - m_nWriteLen;
}

Int32 CBuffer::GetCapacity() const
{
    return m_nCapacity;
}

Int32 CBuffer::GetUnreadLen() const
{
    return m_nWriteLen - m_nReadLen;
}

void CBuffer::Read(Int32 nLen)
{
    Assert(m_nReadLen + nLen <= m_nWriteLen);
    m_nReadLen += nLen;
}

LPSTR CBuffer::GetWritePtr() const
{
    return m_szBuffer + m_nWriteLen;
}

LPSTR CBuffer::GetReadPtr() const
{
    return m_szBuffer + m_nReadLen;
}

LPSTR CBuffer::GetBuffer() const
{
    return m_szBuffer;
}
