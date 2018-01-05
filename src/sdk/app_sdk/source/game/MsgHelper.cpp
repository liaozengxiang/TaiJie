#include <sstream>
#include "MsgHelper.h"
#include "../parser/Parser.h"
using namespace std;

string CMsgHelper::MakeLoginRequest(const char *lpszUserName, const char *lpszPassword)
{
    ostringstream oss;
    oss << "{\"url\": \"/usr/login\", \"login_id\": \"" << lpszUserName << "\", \"password\": \"" << lpszPassword << "\"}";
    string strBody = oss.str();

    string msg = MakeMsgHeader(eMsgType_Request, 1, strBody.length());
    msg.append(strBody);

    return msg;
}

string CMsgHelper::MakeMsgHeader(EMsgType eType, unsigned short uMsgID, unsigned short uSize)
{
    MsgHeader hdr;
    hdr.magic = 'L';
    hdr.message_type = eType;
    hdr.message_id = uMsgID;
    hdr.size = uSize;

    string msg((const char *)&hdr, sizeof(MsgHeader));
    return msg;
}
