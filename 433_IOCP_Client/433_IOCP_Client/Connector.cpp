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

void CConnector::Register(CSockInfo &p_rSock)
{
	ZeroMemory(&p_rSock.m_cAddr, sizeof(p_rSock.m_cAddr));
	p_rSock.m_cAddr.sin_family = AF_INET;
	p_rSock.m_cAddr.sin_addr.s_addr = g_dwIp;
	p_rSock.m_cAddr.sin_port = htons(t_nPort);

	BOOL t_bRet = g_cMswsock.ConnectEx(p_rSock.m_hSock, (SOCKADDR*)&p_rSock.m_cAddr, sizeof(p_rSock.m_cAddr), NULL, 0, NULL, p_rSock.m_vpAct[CSockInfo::ACT_TYPE::ACT_CONNECT]);

	if (!t_bRet)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			g_pLog->myWprintf(L"ConnectEx( ) error on CBotConnector::Register( )\n");
		}
	}
	else
	{
		std::cout << "CConnector::Register has been succeeded immediately !" << std::endl;
	}
}

void CConnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	CSockInfo &t_sMysock = p_pAct->m_cSock;

	m_pProactor->Register((HANDLE)t_sMysock.m_hSock);

	g_pLog->myWprintf(L"The Connection has been made successfully !\n");

	// recv()
}

void CConnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}