#include "../Include/HttpFrameServer.h"
#include "../Include/HttpClientEvent.h"

CHttpFrameServer::CHttpFrameServer(CEventEngine *pListenEngine) : CFrameServer(pListenEngine)
{
	m_pHandlerFactory = NULL;
	m_lLinkTimeout = 60000;
}

CHttpFrameServer::~CHttpFrameServer()
{
}

Bool CHttpFrameServer::Start(const CSocketAddr &address, const std::vector<CEventEngine *> &vecAsyncIo, CThreadPool *pWorkPool, IHttpHandlerFactory *pHandlerFactory)
{
	Assert(pHandlerFactory != NULL);
	m_pHandlerFactory = pHandlerFactory;

	return CFrameServer::Start(address, vecAsyncIo, pWorkPool);
}

void CHttpFrameServer::SetLinkTimeout(Int64 lLinkTimeout)
{
	m_lLinkTimeout = lLinkTimeout;
}

CClientEvent* CHttpFrameServer::CreateClientEvent(Int32 nClientFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher)
{
	return new CHttpClientEvent(nClientFD, addr, pEngine, pDispatcher, m_pHandlerFactory, m_lLinkTimeout);
}
