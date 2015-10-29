#pragma once

class CActor
{
public:
	CActor();
	~CActor();

	virtual void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes) = 0;
	virtual void ProcError(CAct *p_pAct, DWORD p_dwError) = 0;
};

