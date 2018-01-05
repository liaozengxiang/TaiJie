#ifndef MSG__HELPER__20180105
#define MSG__HELPER__20180105

#include <string>
using namespace std;

class CMsgHelper
{
    enum EMsgType
    {
        eMsgType_Request  = 1,
        eMsgType_Response = 2,
        eMsgType_Notice   = 3
    };

public:
    static string MakeLoginRequest(const char *lpszUserName, const char *lpszPassword);

private:
    static string MakeMsgHeader(EMsgType eType, unsigned short uMsgID, unsigned short uSize);
};

#endif
