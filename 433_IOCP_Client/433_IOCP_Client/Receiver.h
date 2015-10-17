#pragma once
class CReceiver : public CActor
{
public:
	CReceiver(CProactor *p_pProactor);
	~CReceiver();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

