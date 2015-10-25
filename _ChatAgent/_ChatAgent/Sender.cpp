#include "stdafx.h"

CSender::CSender(Proactor *p_pProactor)
{
	mProactor = p_pProactor;
}


CSender::~CSender()
{
}

void CSender::ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes)
{

}

void CSender::ProcError(Act *p_pAct, DWORD p_dwError)
{

}