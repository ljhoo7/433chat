#include "stdafx.h"

CAct::CAct(CActor *p_pActor, CBot *p_pBot)
:m_pActor(p_pActor), m_pBot(p_pBot)
{
}

CAct::~CAct()
{
}

bool CAct::Complete(DWORD p_dwTransferredBytes)
{
	m_pActor->ProcEvent(this, p_dwTransferredBytes);

	return true;
}