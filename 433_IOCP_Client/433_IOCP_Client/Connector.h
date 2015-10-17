#pragma once
class CConnector : public CActor
{
public:
	CConnector(CProactor *p_pProactor);
	~CConnector();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);

	void Register(CSockInfo &p_rSock);
};

