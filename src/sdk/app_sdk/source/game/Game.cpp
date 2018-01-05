#include "Game.h"
#include "MsgHelper.h"

CGame::CGame()
{
    m_uPort = 0;
    m_pNotify = NULL;
}

CGame::~CGame()
{
    m_client.Stop();
}

void CGame::SetServer(const char *lpszServer, unsigned short uPort)
{
    m_strServer = lpszServer;
    m_uPort = uPort;
}

void CGame::SetNotification(IGameNotify *pNotify)
{
    m_pNotify = pNotify;
}

bool CGame::Startup()
{
    CSocketAddr addr(m_strServer.c_str(), m_uPort);
    return m_client.Start(addr, this);
}

bool CGame::Login(const char *lpszLoginID, const char *lpszPassword)
{
    string strRequest = CMsgHelper::MakeLoginRequest(lpszLoginID, lpszPassword);

    m_client.Send(strRequest.c_str(), strRequest.length());
    return true;
}

bool CGame::SitDown(int nDesktopID, int nSeatID)
{
    return false;
}

bool CGame::Ready(bool bReady)
{
    return false;
}

bool CGame::Bid(EScore eScore)
{
    return false;
}

bool CGame::PutBottomCard(unsigned char *pCard, int nSize)
{
    return false;
}

bool CGame::SetMasterSuit(ESuit eSuit)
{
    return false;
}

bool CGame::PlayCard(unsigned char *pCard, int nSize)
{
    return false;
}

bool CGame::GetPlayerPoints(const char *lpszUserID)
{
    return false;
}

bool CGame::Leave()
{
    return false;
}

bool CGame::Quit()
{
    return false;
}

void CGame::Release()
{
    delete this;
}

void CGame::OnClientDisconnected()
{
}

Bool CGame::OnParsePacket(const MsgHeader *pMsgHdr, const char *lpszContent, Int32 nLen)
{
    return True;
}

IGame* GetGameInstance()
{
    static CGame s_game;
    return &s_game;
}

int main()
{
    do
    {
        IGame *pGame = GetGameInstance();
        pGame->SetServer("172.30.30.106", 6666);
        if (!pGame->Startup())
        {
            printf("启动失败\n");
            break;
        }

        if (!pGame->Login("liaozengxiang", "lory1234"))
        {
            printf("登录失败\n");
            break;
        }

        printf("登录成功\n");
        while (1)
        {
            usleep(1000*1000);
        }
    } while (0);

    return 0;
}
