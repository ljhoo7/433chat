#include "stdafx.h"

CBotReceiver::CBotReceiver(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}

CBotReceiver::~CBotReceiver()
{
}

void CBotReceiver::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{

}

void CBotReceiver::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}