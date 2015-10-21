#pragma once

class Connector : public Actor
{
public:
	Connector(Proactor *p_pProactor);
	~Connector();

	void ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(Act *p_pAct, DWORD p_dwError);
};

