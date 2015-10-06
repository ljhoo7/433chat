#include "stdafx.h"

extern CLogWriter *g_pLog;

CSockInfo::CSockInfo(CProactor *p_pProactor, CBotConnector *p_pBotConnector, CBotReceiver *p_pBotReceiver
	, CBotSender *p_pBotSender)
	: m_pProactor(p_pProactor), m_pBotConnector(p_pBotConnector), m_pBotReceiver(p_pBotReceiver)
	, m_pBotSender(p_pBotSender)
{
	CAct *t_cAct;
	t_cAct = new CAct(p_pBotConnector);
	m_vpAct.push_back(t_cAct);
	t_cAct = new CAct(p_pBotReceiver);
	m_vpAct.push_back(t_cAct);
	t_cAct = new CAct(p_pBotSender);
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
}