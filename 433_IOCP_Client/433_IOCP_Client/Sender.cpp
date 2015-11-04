#include "stdafx.h"

extern CLogWriter *g_pLog;

CSender::CSender(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}


CSender::~CSender()
{
}

void CSender::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
}

void CSender::ProcError(CAct *p_pAct, DWORD p_dwError)
{
	g_pLog->myWprintf(L"error! Sender's ProError!\n");
}