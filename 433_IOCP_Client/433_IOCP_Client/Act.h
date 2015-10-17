#pragma once

struct CAct : public OVERLAPPED
{
	CSockInfo m_cSock;
	CActor	  *m_pActor;

	CAct(CActor *p_pActor)
		:m_pActor(p_pActor)
	{
		hEvent = NULL;
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;
	}

	CAct(CProactor *p_pProactor, CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver
		, CSender *p_pSender)
	: m_cSock(p_pProactor, p_pConnector, p_pDisconnector, p_pReceiver, p_pSender)
	{
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
};