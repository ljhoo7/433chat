#pragma once
class CBotReceiver : public CActor
{
public:
	CBotReceiver(CProactor *p_pProactor);
	~CBotReceiver();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

