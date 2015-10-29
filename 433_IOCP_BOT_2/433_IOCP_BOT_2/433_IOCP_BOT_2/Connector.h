#pragma once

class CConnector : public CActor
{
	CProactor		*m_pProactor;
public:
	CConnector(CProactor *p_pProactor);
	~CConnector();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

