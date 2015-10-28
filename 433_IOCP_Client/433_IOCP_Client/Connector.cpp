#include "stdafx.h"

extern mswsock_s g_cMswsock;
extern CLogWriter *g_pLog;



extern DWORD		g_dwIp;
extern int			t_nPort;

extern CClient		*g_pClient;

CConnector::CConnector(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}


CConnector::~CConnector()
{
}

void CConnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	if (CEscaping::Instance() != g_pClient->GetStateMachine()->CurrentState())
	{
		CSockInfo &t_sMysock = *p_pAct->m_pSock;

		g_pClient->m_pSock->m_hSock = g_pClient->m_hNewSock;
		g_pLog->myWprintf(L"The First Connection has been made successfully !\n");

		t_sMysock.Recv(t_sMysock.m_szReceiveBuf, HEADER_SIZE);
	}
	else
	{
		g_pLog->myWprintf(L"The Escaping Connection has been made successfully !\n");

		g_pClient->SendEscapeSuccessMessage();

		g_pClient->GetStateMachine()->SetCurrentState(CEscaping::Instance());
	}
}

void CConnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{
	g_pLog->myWprintf(L"The Escaping Connection has been failed !\n");
}