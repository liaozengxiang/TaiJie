#ifndef CLIENT__20171220
#define CLIENT__20171220

#include <Common/Common.h>
#include "../parser/Parser.h"

struct IClientCallback
{
    virtual void OnClientDisconnected() = 0;
    virtual Bool OnParsePacket(const MsgHeader *pMsgHdr, const char *lpszContent, Int32 nLen) = 0;
};

class CClient : public ITCPClientSocketEvent, public CEvent, public IMessageEvent, public IParserCallback
{
public:
    CClient();
    virtual ~CClient();

public:
    Bool Start(const CSocketAddr &server, IClientCallback *pCallback);
    void Send(const char *lpszBuf, Int32 nLen);
    void Stop();

protected:
    // ITCPClientSocketEvent
    virtual void OnConnected(CTCPClientSocket *pSocket);
    virtual void OnConnectFailed(CTCPClientSocket *pSocket, Int32 nError);
    virtual void OnConnectTimeout(CTCPClientSocket *pSocket);

    // CEvent
    virtual Int32 GetFD() const;
    virtual void OnRead();
    virtual void OnWrite();
    virtual void OnError(Int32 nErrCode);

    // IMessageEvent
    virtual void OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam);

    // IParserCallback
    virtual Bool OnParsePacket(const MsgHeader *pMsgHdr, const char *lpszContent, Int32 nLen);

protected:
    IClientCallback*            m_pCallback;
    CTCPClientSocket*           m_sock;
    CEventThread                m_thread;
    Int32                       m_nFD;

    list<CBuffer*>              m_lstBuffers;
    CParser                     m_parser;
};

#endif
