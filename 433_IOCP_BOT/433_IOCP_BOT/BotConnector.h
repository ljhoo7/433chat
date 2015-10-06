#pragma once
class CBotConnector : public CActor
{
public:
	CBotConnector(CProactor *p_pProactor);
	~CBotConnector();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);

	void Register(CSockInfo &p_rSock);
};

