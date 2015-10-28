#pragma once

class CDisconnector : public CActor
{
	CProactor		*m_pProactor;
public:
	CDisconnector(CProactor *p_pProactor);
	~CDisconnector();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

