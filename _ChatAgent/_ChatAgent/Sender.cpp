#include "stdafx.h"

Sender::Sender(Proactor *p_pProactor)
{
	mProactor = p_pProactor;
}


Sender::~Sender()
{
}

void Sender::ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes)
{

}

void Sender::ProcError(Act *p_pAct, DWORD p_dwError)
{

}