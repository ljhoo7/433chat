#include "stdafx.h"

extern mswsock_s g_cMswsock;
extern CLogWriter *g_pLog;
extern DWORD g_dwIp;
extern int t_nPort;

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

	g_pLog->myWprintf(L"The Connection has been made successfully !\n");

	t_sMysock.Recv(t_sMysock.m_szReceiveBuf, HEADER_SIZE);
}

void CConnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}