#include "stdafx.h"

extern CLogWriter *g_pLog;

extern mswsock_s g_cMswsock;

extern DWORD g_dwIp;

extern int t_nPort;

CBotConnector::CBotConnector(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}

CBotConnector::~CBotConnector()
{
}

void CBotConnector::Register(CSockInfo &p_rSock)
{
	ZeroMemory(&p_rSock.m_cAddr, sizeof(p_rSock.m_cAddr));
	p_rSock.m_cAddr.sin_family = AF_INET;
	p_rSock.m_cAddr.sin_addr.s_addr = g_dwIp;
	p_rSock.m_cAddr.sin_port = htons(t_nPort);

	BOOL t_bRet = g_cMswsock.ConnectEx(p_rSock.m_hSock, (SOCKADDR*)&p_rSock.m_cAddr, sizeof(p_rSock.m_cAddr), NULL, 0, NULL, p_rSock.m_vpAct[CSockInfo::ACT_TYPE::ACT_BOTCONNECT]);

	if (!t_bRet)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			g_pLog->myWprintf(L"ConnectEx( ) error on CBotConnector::Register( )\n");
		}
	}
}

void CBotConnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{

}

void CBotConnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}