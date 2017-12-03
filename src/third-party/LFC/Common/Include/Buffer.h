/*
 * Buffer.h
 *
 *  Created on: Jun 01, 2016
 *      Author: lory
 */

#ifndef ___BUFFER__20150611___
#define ___BUFFER__20150611___

#include "OSType.h"

class CBuffer
{
public:
    CBuffer();
    virtual ~CBuffer();

public:
    Bool  Initialize(Int32 nSize);
    Bool  Initialize(LPSTR szBuffer, Int32 nSize);
    void  Clear();
    Int32 Write(LPCSTR lpszBuffer, Int32 nLen);
    Int32 Write(Int32 nLen);
    Int32 GetReadLen() const;
    Int32 GetWriteLen() const;
    Int32 GetSpace() const;
    Int32 GetCapacity() const;
    Int32 GetUnreadLen() const;
    void  Read(Int32 nLen);
    LPSTR GetWritePtr() const;
    LPSTR GetReadPtr() const;
    LPSTR GetBuffer() const;

protected:
    Bool                        m_bOwnBuffer;
    LPSTR                       m_szBuffer;
    Int32                       m_nCapacity;
    Int32                       m_nWriteLen;
    Int32                       m_nReadLen;
};

#endif
