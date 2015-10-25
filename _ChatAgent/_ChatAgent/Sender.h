#pragma once
class CSender : public Actor
{
public:
	CSender(Proactor *p_pProactor);
	~CSender();

	void ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(Act *p_pAct, DWORD p_dwError);
};

