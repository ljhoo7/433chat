#include "stdafx.h"

CDisconnector::CDisconnector(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}


CDisconnector::~CDisconnector()
{
}

void CDisconnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{

}

void CDisconnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}
