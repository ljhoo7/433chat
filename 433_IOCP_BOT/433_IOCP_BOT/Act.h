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

	CAct(CProactor *p_pProactor, CBotConnector *p_pBotConnector, CBotReceiver *p_pBotReceiver
		, CBotSender *p_pBotSender)
	: m_cSock(p_pProactor, p_pBotConnector, p_pBotReceiver, p_pBotSender)
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