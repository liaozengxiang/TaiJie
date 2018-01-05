#ifndef GAME__20171201
#define GAME__20171201

#include <string>
#include "../../include/IGame.h"
#include "../socket/Client.h"
using namespace std;

class CGame : public IGame, public IClientCallback
{
public:
    CGame();
    virtual ~CGame();

protected:
    virtual void SetServer(const char *lpszServer, unsigned short uPort);
    virtual void SetNotification(IGameNotify *pNotify);
    virtual bool Login(const char *lpszLoginID, const char *lpszPassword);
    virtual bool SitDown(int nDesktopID, int nSeatID);
    virtual bool Ready(bool bReady);
    virtual bool Bid(EScore eScore);
    virtual bool PutBottomCard(unsigned char *pCard, int nSize);
    virtual bool SetMasterSuit(ESuit eSuit);
    virtual bool PlayCard(unsigned char *pCard, int nSize);
    virtual bool GetPlayerPoints(const char *lpszUserID);
    virtual bool Leave();
    virtual bool Quit();
    virtual void Release();

protected:
    virtual void OnClientDisconnected();
    virtual Bool OnParsePacket(const MsgHeader *pMsgHdr, const char *lpszContent, Int32 nLen);

protected:
    string          m_strServer;
    unsigned short  m_uPort;
    IGameNotify*    m_pNotify;

    string          m_strLoginID;
    string          m_strPassword;

    CClient         m_client;
};

#endif
