#pragma once

class CAct : public OVERLAPPED
{
public:
	pkt_type		m_eType;

	CSockInfo		*m_pSock, *m_pPrevSock;

	CActor			*m_pActor;

	CAct() : m_eType(pkt_type::pt_default) {}

	CAct(CActor *p_pActor)
		:m_pActor(p_pActor), m_pPrevSock(NULL)
	{
		hEvent = NULL;
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;
	}

	CAct(CProactor *p_pProactor, CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver
		, CSender *p_pSender)
		: m_eType(pkt_type::pt_default), m_pPrevSock(NULL)
	{
		m_pSock = new CSockInfo(p_pProactor, p_pConnector, p_pDisconnector, p_pReceiver, p_pSender);

		m_pActor = NULL;
		hEvent = NULL;
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;
	}

	void Complete(DWORD p_dwBytesTransferred)
	{
		m_pActor->ProcEvent(this, p_dwBytesTransferred);
	}

	void Error(DWORD p_dwError)
	{
		m_pActor->ProcError(this, p_dwError);
	}

	void Init(CActor *p_pActor, CSockInfo *p_pSockInfo)
	{
		m_pActor = p_pActor;
		m_pSock = p_pSockInfo;
	}
};