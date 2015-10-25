#pragma once
class CSender : public CActor
{
public:
	CSender(CProactor *p_pProactor);
	~CSender();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

