#pragma once
class Sender : public Actor
{
public:
	Sender(Proactor *p_pProactor);
	~Sender();

	void ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(Act *p_pAct, DWORD p_dwError);
};

