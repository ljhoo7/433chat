#pragma once
class CReceiver : public CActor
{
public:

	bool(*CReceiver::PacketHandlingFunc[pkt_type::pt_default])(PVOID p_pParam);

	CReceiver(CProactor *p_pProactor);
	~CReceiver();

	void ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes);
	void ProcError(CAct *p_pAct, DWORD p_dwError);
};

