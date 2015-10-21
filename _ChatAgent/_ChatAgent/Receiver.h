#pragma once
class CReceiver : public Actor
{
public:
	CReceiver(Proactor *p_pProactor);
	~CReceiver();

	void ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(Act *p_pAct, DWORD p_dwError);
};

