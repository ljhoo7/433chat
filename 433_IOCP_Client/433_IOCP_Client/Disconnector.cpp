#include "stdafx.h"

extern CClient		*g_pClient;

CDisconnector::CDisconnector(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}


CDisconnector::~CDisconnector()
{
}

void CDisconnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	if (g_pClient->m_hNewSock != NULL){
		g_pClient->m_pSock->m_hSock = g_pClient->m_hNewSock;
		g_pClient->m_hNewSock = NULL;

		(g_pClient->m_pSock)->Recv((g_pClient->m_pSock)->m_szReceiveBuf, HEADER_SIZE);
	}
	printf("disconnect success!\n");
}

void CDisconnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{
	printf("disconnect error!\n");
}
