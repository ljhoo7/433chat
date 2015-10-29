#include "stdafx.h"

extern CLogWriter		*g_pLog;

CConnector::CConnector(CProactor *p_pProactor)
:m_pProactor(p_pProactor)
{
}


CConnector::~CConnector()
{
}

void CConnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	bool &t_bEscaping = p_pAct->m_pBot->m_bIsEscaping;
	CBot *t_cBot = p_pAct->m_pBot;

	if (t_bEscaping)
	{
		
		g_pLog->myWprintf("The Escaping Connection Success !!!\n");
	}
	else
	{
		t_cBot->m_hSock = t_cBot->m_hNewSock;

		g_pLog->myWprintf("The First Connection Success !!!\n");

		t_cBot->Recv(t_cBot->m_szReceiveBuf, HEADER_SIZE);
	}
}

void CConnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{
	g_pLog->myWprintf("The Connection has been failed !\n");
}