#pragma once


class Receiver : public Actor
{
public:
	Receiver(Proactor *p_pProactor);
	~Receiver();

	void ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(Act *p_pAct, DWORD p_dwError);
};

