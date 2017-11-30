/*
| -------------------------------------------------------------------
| IGame.h
| -------------------------------------------------------------------
| 本文件定义了客户端游戏操作接口
|
*/

#ifndef IGAME__20171129
#define IGAME__20171129

#ifndef TAIJIE_API
#define TAIJIE_API extern "C" __declspec(dllimport)
#endif

// 叫分的枚举定义
enum EScore
{
    eScore_Pass = 0,                // 不要
    eScore_80,
    eScore_85,
    eScore_90,
    eScore_95,
    eScore_100,
    eScore_105,
    eScore_110,
    eScore_115,
    eScore_120,
    eScore_125,                     // 台阶
    eScore_130,
    eScore_135,
    eScore_140,
    eScore_145,
    eScore_150,
    eScore_155,
    eScore_160,
    eScore_165,
    eScore_170,
    eScore_175,
    eScore_Max
};

// 花色的枚举定义
enum ESuit
{
    eSuit_Spade = 1,        // 黑桃
    eSuit_Heart,            // 红桃
    eSuit_Club,             // 梅花
    eSuit_Diamond           // 方块
};

struct IGameNotify
{
    virtual void OnUserIn(const char *lpszUserID, int nSeatID) = 0;                         // 用户进来
    virtual void OnUserLeave(const char *lpszUserID) = 0;                                   // 用户离开
    virtual void OnUserReady(bool bReady) = 0;                                              // 用户准备/取消准备
    virtual void OnGameBegin() = 0;                                                         // 游戏开始
    virtual void OnDeal(const char *lpszUserID, unsigned char *pCard, int nSize) = 0;       // 发牌，每轮共回调4次
    virtual void OnTurnToBid(const char *lpszUserID, int nCountDown) = 0;                   // 轮到叫分，并指定倒计时秒数
    virtual void OnBid(const char *lpszUserID, EScore eScore) = 0;                          // 叫分
    virtual void OnBankerConfirmed(const char *lpszUserID, int nScoreNeeded) = 0;           // 确认庄家，并指定闲家需要上分的总数
    virtual void OnPutBottomCard(unsigned char *pCard, int nSize) = 0;                      // 庄家放底牌
    virtual void OnSuitConfirmed(ESuit eSuit) = 0;                                          // 庄家确定主花色
    virtual void OnTurnToPlayCard(const char *lpszUserID, int nCountDown) = 0;              // 轮到指定玩家出牌，并指定倒计时秒数
    virtual void OnPlayCard(const char *lpszUserID, unsigned char *pCard, int nSize) = 0;   // 玩家出牌
    virtual void OnPlayerScore(int nStepScore, int nTotalScore);                            // 闲家上分
    virtual void OnScoreExceeded() = 0;                                                     // 闲家已破分
    virtual void OnGameOver(int nScoreNeeded, int nTotalScore, int nPoints) = 0;            // 游戏结束，判定胜负，确定本轮的点数
};

struct IGame
{
    virtual void SetServer(const char *lpszServer, unsigned short uPort) = 0;               // 设置服务器地址
    virtual void SetNotification(IGameNotify *pNotify) = 0;                                 // 设置通知接口
    virtual bool Login(const char *lpszLoginID, const char *lpszPassword) = 0;              // 登录
    virtual bool SeatDown(int nDesktopID, int nSeatID) = 0;                                 // 入座
    virtual bool Ready(bool bReady) = 0;                                                    // 准备/取消准备
    virtual bool Bid(EScore eScore) = 0;                                                    // 叫分
    virtual bool PutBottomCard(unsigned char *pCard, int nSize) = 0;                        // 放底牌，数量必须是8张
    virtual bool SetMasterSuit(ESuit eSuit) = 0;                                            // 设置主花色
    virtual bool PlayCard(unsigned char *pCard, int nSize) = 0;                             // 出牌
    virtual bool GetPlayerPoints(const char *lpszUserID, int *pPoints,
                                 int *pWinCount, int *pLoseCount) = 0;                      // 获取玩家的战况
    virtual bool Leave() = 0;                                                               // 玩家离开游戏桌
    virtual bool Quit() = 0;                                                                // 玩家登出游戏
    virtual void Release() = 0;                                                             // 释放游戏对象
};

TAIJIE_API IGame* GetGameInstance();

#endif
