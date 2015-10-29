#pragma once

class CReceiver : public CActor
{
	bool(*CReceiver::PacketHandlingFunc[pkt_type::pt_default])(PVOID p_pParam);

	CProactor		*m_pProactor;
public:
	CReceiver(CProactor *p_pProactor);
	~CReceiver();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

