#include "stdafx.h"

extern CLogWriter *g_pLog;

CSockInfo::CSockInfo(CProactor *p_pProactor, CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver, CSender *p_pSender)
	: m_pProactor(p_pProactor), m_pConnector()
{
	CAct *t_cAct;
	t_cAct = new CAct(p_pConnector);
	m_vpAct.push_back(t_cAct);
	t_cAct = new CAct(p_pDisconnector);
	m_vpAct.push_back(t_cAct);
	t_cAct = new CAct(p_pReceiver);
	m_vpAct.push_back(t_cAct);
	t_cAct = new CAct(p_pSender);
	m_vpAct.push_back(t_cAct);

	m_hSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (m_hSock == INVALID_SOCKET)
	{
		printf("WSASocket() Error!!! err(%d)\n", WSAGetLastError());
	}

	ZeroMemory(&m_cAddr, sizeof(m_cAddr));
	m_cAddr.sin_family = AF_INET;
	m_cAddr.sin_addr.s_addr = INADDR_ANY;
	m_cAddr.sin_port = 0;
	int t_nRc = bind(m_hSock, (SOCKADDR*)&m_cAddr, sizeof(m_cAddr));
	if (t_nRc != 0) {
		g_pLog->myWprintf(L"bind error in CSockInfo constructor\n");
	}

	m_wsaRecvBuf.buf = (char*)m_szReciveBuf;
	m_wsaRecvBuf.len = BUFSIZE;

	m_wsaSendBuf.buf = (char*)m_szSendBuf;
	m_wsaSendBuf.len = BUFSIZE;

	ZeroMemory(m_szReciveBuf, BUFSIZE);
	ZeroMemory(m_szSendBuf, BUFSIZE);
	ZeroMemory(m_szConnectBuf, BUFSIZE);

	p_pProactor->Register((HANDLE)m_hSock);
}