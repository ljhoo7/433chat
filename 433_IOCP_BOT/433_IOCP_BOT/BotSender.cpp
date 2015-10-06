#include "stdafx.h"

CBotSender::CBotSender(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}

CBotSender::~CBotSender()
{
}

void CBotSender::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{

}

void CBotSender::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}