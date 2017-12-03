#ifndef RPC__20171203
#define RPC__20171203

#include <Common/Common.h>

class CRpc
{
public:
    void SetServer(LPCSTR lpszServer, UInt16 uPort, Bool bSSL);
    Bool Invoke(LPCSTR lpszUri, LPCSTR lpszInput, LPCSTR lpszMimeType);
    string GetOutput() const;
    string GetOutputMimeType() const;
};

#endif
