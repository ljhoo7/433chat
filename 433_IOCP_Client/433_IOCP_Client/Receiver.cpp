#include "stdafx.h"

CReceiver::CReceiver(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}


CReceiver::~CReceiver()
{
}

void CReceiver::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{

}

void CReceiver::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}