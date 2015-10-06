#pragma once
class CBotSender : public CActor
{
public:
	CBotSender(CProactor *p_pProactor);
	~CBotSender();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

