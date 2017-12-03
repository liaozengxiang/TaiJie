#include "../Include/RtspFrameServer.h"
#include "../Include/RtspClientEvent.h"

CRtspFrameServer::CRtspFrameServer(CEventEngine *pListenEngine) : CFrameServer(pListenEngine)
{
	m_pHandlerFactory = NULL;
	m_lLinkTimeout = 60000;
}

CRtspFrameServer::~CRtspFrameServer()
{
}

Bool CRtspFrameServer::Start(const CSocketAddr &address, const std::vector<CEventEngine *> &vecAsyncIo, CThreadPool *pWorkPool, IRtspHandlerFactory *pHandlerFactory)
{
	Assert(pHandlerFactory != NULL);
	m_pHandlerFactory = pHandlerFactory;

	return CFrameServer::Start(address, vecAsyncIo, pWorkPool);
}

void CRtspFrameServer::SetLinkTimeout(Int64 lLinkTimeout)
{
	m_lLinkTimeout = lLinkTimeout;
}

CClientEvent* CRtspFrameServer::CreateClientEvent(Int32 nClientFD, const CSocketAddr &addr, CEventEngine *pEngine, IMessageDispatcher *pDispatcher)
{
	return new CRtspClientEvent(nClientFD, addr, pEngine, pDispatcher, m_pHandlerFactory, m_lLinkTimeout);
}
