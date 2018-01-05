#ifndef PARSER__20171220
#define PARSER__20171220

#include <Common/Common.h>
#include <string>
using namespace std;

#pragma pack(1)

struct MsgHeader
{
    unsigned char   magic;          // 协议标识，必须为 'L'
    unsigned char   message_type;   // 消息类型: 1=请求; 2=响应; 3=通知
    unsigned short  message_id;     // 消息的标识，对于请求消息，服务器端会透传回来
    unsigned short  size;           // 消息体的长度，消息体为 json 格式
};

#pragma pack()

struct IParserCallback
{
    virtual Bool OnParsePacket(const MsgHeader *pMsgHdr, const char *lpszContent, Int32 nLen) = 0;
};

class CParser
{
public:
    CParser();
    ~CParser();

public:
    void Initialize(IParserCallback *pCallback);
    Bool Parse(const char *lpszBuf, Int32 nLen);

protected:
    IParserCallback*    m_pCallback;
    MsgHeader           m_hdr;
    Int32               m_nRecvHdrLen;
    string              m_strContent;

    enum EState
    {
        eState_ParseHdr = 1,
        eState_ParseContent
    };
    EState              m_eState;
};

#endif
