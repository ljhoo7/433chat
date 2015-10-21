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
	CSockInfo &t_sMysock = *p_pAct->m_pSock;

	//if (CEscaping::Instance() != g_pClient->GetStateMachine()->CurrentState())
	if (!g_pClient->m_bIsEscaping)
	{
		g_pLog->myWprintf(L"The First Connection has been made successfully !\n");

		t_sMysock.Recv(t_sMysock.m_szReceiveBuf, HEADER_SIZE);
	}
	else
	{
		g_pLog->myWprintf(L"The Escaping Connection has been made successfully !\n");

		g_pClient->SendEscapeSuccessMessage();

		g_pClient->m_bIsEscaping = false;

		//t_sMysock.Disconnect();
	}
}

void CConnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{
	CSockInfo &t_sMysock = *p_pAct->m_pSock;

	//if (CEscaping::Instance() == g_pClient->GetStateMachine()->CurrentState())
	{
		g_pLog->myWprintf(L"The Escaping Connection has been failed !\n");

		//g_pClient->SendEscapeFailureMessage();
	}
}