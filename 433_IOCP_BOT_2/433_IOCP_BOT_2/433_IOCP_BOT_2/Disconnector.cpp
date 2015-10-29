#include "stdafx.h"

extern CLogWriter		*g_pLog;

CDisconnector::CDisconnector(CProactor *p_pProactor)
:m_pProactor(p_pProactor)
{
}


CDisconnector::~CDisconnector()
{
}

void CDisconnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	SOCKET &t_hSock = p_pAct->m_pBot->m_hSock;
	if (t_hSock != NULL){
		t_hSock = p_pAct->m_pBot->m_hNewSock;
		p_pAct->m_pBot->m_hNewSock = NULL;

		p_pAct->m_pBot->Recv(p_pAct->m_pBot->m_szReceiveBuf, HEADER_SIZE);
	}
	printf("disconnect success!\n");
}

void CDisconnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{
	printf("disconnect error!\n");
}