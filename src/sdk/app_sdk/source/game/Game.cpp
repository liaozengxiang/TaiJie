#include "Game.h"

CGame::CGame()
{
    m_uPort = 0;
    m_pNotify = NULL;
}

CGame::~CGame()
{
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

bool CGame::Login(const char *lpszLoginID, const char *lpszPassword)
{
    return false;
}

bool CGame::SeatDown(int nDesktopID, int nSeatID)
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

bool CGame::GetPlayerPoints(const char *lpszUserID, int *pPoints, int *pWinCount, int *pLoseCount)
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
        if (!pGame->Login("liaozengxiang", "lory1234"))
        {
            printf("登录失败\n");
            break;
        }

        printf("登录成功\n");
    } while (0);

    return 0;
}
