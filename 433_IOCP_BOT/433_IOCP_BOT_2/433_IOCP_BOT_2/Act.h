#pragma once

class CAct : public OVERLAPPED
{
public:
	CBot				*m_pBot;
	CActor				*m_pActor;

	CAct(CActor *p_pActor, CBot *p_pBot);
	~CAct();

	bool Complete(DWORD p_dwTransferredBytes);
};
