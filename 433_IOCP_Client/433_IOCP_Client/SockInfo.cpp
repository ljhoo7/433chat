#include "stdafx.h"

extern mswsock_s g_cMswsock;
extern CLogWriter *g_pLog;
extern CClient *g_pClient;

void CSockInfo::BindBeforeConnectEx()
{
	g_pClient->m_hNewSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (g_pClient->m_hNewSock == INVALID_SOCKET)
	{
		printf("WSASocket() Error!!! err(%d)\n", WSAGetLastError());
	}

	ZeroMemory(&m_cAddr, sizeof(m_cAddr));
	m_cAddr.sin_family = AF_INET;
	m_cAddr.sin_addr.s_addr = INADDR_ANY;
	m_cAddr.sin_port = 0;

	int t_nRc;
	t_nRc = bind(g_pClient->m_hNewSock, (SOCKADDR*)&m_cAddr, sizeof(m_cAddr));

	if (t_nRc != 0) {
		g_pLog->myWprintf(L"bind error in CSockInfo constructor\n");

		exit(0);
	}

	m_wsaRecvBuf.buf = (char*)m_szReceiveBuf;
	m_wsaRecvBuf.len = BUFSIZE;

	m_wsaSendBuf.buf = (char*)m_szSendBuf;
	m_wsaSendBuf.len = BUFSIZE;

	ZeroMemory(m_szReceiveBuf, BUFSIZE);
	ZeroMemory(m_szSendBuf, BUFSIZE);
	ZeroMemory(m_szConnectBuf, BUFSIZE);

	m_pProactor->Register((HANDLE)g_pClient->m_hNewSock);
}

CSockInfo::CSockInfo(CProactor *p_pProactor, CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver, CSender *p_pSender)
: m_pProactor(p_pProactor), m_pConnector(p_pConnector), m_pDisconnector(p_pDisconnector)
, m_pReceiver(p_pReceiver), m_pSender(p_pSender), m_nRecvPosition(0), m_nRecvRemain(HEADER_SIZE)
, m_bIsVar(false), m_hSock(NULL)
{
	m_vAct[0] = new CAct();
	m_vAct[1] = new CAct();
	m_vAct[2] = new CAct();
	m_vAct[3] = new CAct();

	m_vAct[0]->Init(p_pConnector, this);
	m_vAct[1]->Init(p_pDisconnector, this);
	m_vAct[2]->Init(p_pReceiver, this);
	m_vAct[3]->Init(p_pSender, this);
}

bool CSockInfo::Connect(DWORD ip, int port)
{
	BOOL t_bRet;

	// This must be here !!!
	// m_hSock = g_pClient->m_pSock->m_hSock;

	BindBeforeConnectEx();

	ZeroMemory(&m_cAddr, sizeof(m_cAddr));
	m_cAddr.sin_family = AF_INET;
	m_cAddr.sin_addr.s_addr = ip;
	m_cAddr.sin_port = htons(port);

	t_bRet = g_cMswsock.ConnectEx(g_pClient->m_hNewSock, (SOCKADDR*)&m_cAddr, sizeof(m_cAddr), NULL, 0, NULL, static_cast<LPOVERLAPPED>(m_vAct[CSockInfo::ACT_TYPE::ACT_CONNECT]));

	if (!t_bRet)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			g_pLog->myWprintf(L"ConnectEx( ) error on CSockInfo::Connect( )\n");

			return false;
		}
	}
	else
	{
		g_pLog->myWprintf(L"CConnector::Register has been succeeded immediately !\n");

		return false;
	}

	return true;
}

bool CSockInfo::Disconnect()
{
	closesocket(m_hSock);
	g_pClient->m_pSock->m_hSock = NULL;

	if (g_pClient->m_hNewSock != NULL){
		g_pClient->m_pSock->m_hSock = g_pClient->m_hNewSock;
		g_pClient->m_hNewSock = NULL;

		(g_pClient->m_pSock)->Recv((g_pClient->m_pSock)->m_szReceiveBuf, HEADER_SIZE);
	}
	printf("disconnect success!\n");

	return true;
}

bool CSockInfo::Recv(char *p_pBuf, int p_nBufSize)
{
	DWORD recvbytes = 0;
	DWORD flags = 0;
	m_wsaRecvBuf.buf = p_pBuf;
	m_wsaRecvBuf.len = p_nBufSize;

	INT ret;

	ret = WSARecv(g_pClient->m_pSock->m_hSock, &m_wsaRecvBuf, 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(m_vAct[ACT_RECV]), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			g_pLog->myWprintf(L"WSARecv() Error!!!\n");
			//Disconnect();

			return false;
		}
	}

	//g_pLog->myWprintf(L"%d bytes were received !\n", recvbytes);

	return true;
}

bool CSockInfo::Send(char *p_pBuf, int p_nBufLen)
{
	if (p_nBufLen == 0)
	{
		g_pLog->myWprintf(L"p_nBufLen is zero in Send function.\n");
		return false;
	}

	DWORD sentbytes = 0;
	m_wsaSendBuf.buf = p_pBuf;
	m_wsaSendBuf.len = p_nBufLen;

	unsigned short t_buf = *(unsigned short*)p_pBuf;

	INT ret;

	ret = WSASend(g_pClient->m_pSock->m_hSock, &m_wsaSendBuf, 1, &sentbytes, 0, static_cast<OVERLAPPED*>(m_vAct[ACT_SEND]), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			g_pLog->myWprintf(L"WSASend() Error!!!");

			return false;
		}
	}

	//g_pLog->myWprintf(L"%d bytes were sent !\n", sentbytes);

	return true;
}