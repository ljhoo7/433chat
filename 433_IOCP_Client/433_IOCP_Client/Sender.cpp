#include "stdafx.h"

CSender::CSender(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}


CSender::~CSender()
{
}

void CSender::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	printf("\n");
}

void CSender::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}