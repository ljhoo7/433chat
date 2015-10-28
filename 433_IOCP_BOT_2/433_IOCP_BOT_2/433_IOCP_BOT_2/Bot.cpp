#include "stdafx.h"

extern CLogWriter		*g_pLog;

bool CBot::BindBeforeConnectEx()
{
	int t_nRetval;

	m_hNewSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_hNewSock == INVALID_SOCKET)
	{
		printf("WSASocket() Error!!! err(%d)\n", WSAGetLastError());
		return false;
	}

	ZeroMemory(&m_cAddr, sizeof(m_cAddr));
	m_cAddr.sin_family = AF_INET;
	m_cAddr.sin_addr.s_addr = INADDR_ANY;
	m_cAddr.sin_port = 0;

	t_nRetval = bind(m_hNewSock, (SOCKADDR*)&m_cAddr, sizeof(m_cAddr));

	if (t_nRetval != 0) {
		g_pLog->myWprintf("bind error in CSockInfo constructor\n");

		return false;
	}

	m_wsaRecvBuf.buf = (char*)m_szReceiveBuf;
	m_wsaRecvBuf.len = BUFSIZE;

	m_wsaSendBuf.buf = (char*)m_szSendBuf;
	m_wsaSendBuf.len = BUFSIZE;

	ZeroMemory(m_szReceiveBuf, BUFSIZE);
	ZeroMemory(m_szSendBuf, BUFSIZE);
	ZeroMemory(m_szConnectBuf, BUFSIZE);

	return true;
}

bool CBot::Init(CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver, CSender *p_pSender)
{
	m_vActs[ACT_TYPE::ACT_CONNECT] = new CAct(p_pConnector, this);
	m_vActs[ACT_TYPE::ACT_DISCONNECT] = new CAct(p_pDisconnector, this);
	m_vActs[ACT_TYPE::ACT_RECEIVE] = new CAct(p_pReceiver, this);
	m_vActs[ACT_TYPE::ACT_SEND] = new CAct(p_pSender, this);

	return true;
}

bool CBot::Connect(mswsock_s &p_cConnEx, DWORD p_dwIp, int p_nPort)
{
	BOOL t_bRet;

	ZeroMemory(&m_cAddr, sizeof(m_cAddr));
	m_cAddr.sin_family = AF_INET;
	m_cAddr.sin_addr.s_addr = p_dwIp;
	m_cAddr.sin_port = htons(p_nPort);

	t_bRet = p_cConnEx.ConnectEx(m_hNewSock, (SOCKADDR*)&m_cAddr, sizeof(m_cAddr), NULL, 0, NULL, static_cast<LPOVERLAPPED>(m_vActs[ACT_TYPE::ACT_CONNECT]));

	if (!t_bRet)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			g_pLog->myWprintf("ConnectEx( ) error on CSockInfo::Connect( )\n");

			return false;
		}
	}
	else
	{
		g_pLog->myWprintf("CConnector::Register has been succeeded immediately !\n");

		return false;
	}

	return true;
}

bool CBot::Disconnect()
{
	closesocket(m_hSock);

	m_hSock = NULL;

	if (m_hNewSock != NULL){
		m_hSock = m_hNewSock;
		m_hNewSock = NULL;

		Recv(m_szReceiveBuf, HEADER_SIZE);
	}

	printf("disconnect success!\n");

	return true;
}

StateMachine<CBot> *CBot::GetStateMachine()
{
	if (m_pStateMachine == NULL)
	{
		g_pLog->myWprintf("Error : m_pStateMachine is nullptr !\n");

		return NULL;
	}
	else
	{
		return m_pStateMachine;
	}
}

bool CBot::Recv(char *p_pBuf, int p_nBufSize)
{
	DWORD recvbytes = 0;
	DWORD flags = 0;
	m_wsaRecvBuf.buf = p_pBuf;
	m_wsaRecvBuf.len = p_nBufSize;

	INT ret;

	ret = WSARecv(m_hSock, &m_wsaRecvBuf, 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(m_vActs[ACT_TYPE::ACT_RECEIVE]), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			g_pLog->myWprintf("WSARecv() Error!!!\n");

			return false;
		}
	}

	g_pLog->myWprintf("%d bytes were received !\n", recvbytes);

	return true;
}

bool CBot::Send(char *p_pBuf, int p_nBufLen)
{
	if (p_nBufLen == 0)
	{
		g_pLog->myWprintf("p_nBufLen is zero in Send function.\n");
		return false;
	}

	DWORD sentbytes = 0;
	m_wsaSendBuf.buf = p_pBuf;
	m_wsaSendBuf.len = p_nBufLen;

	unsigned short t_buf = *(unsigned short*)p_pBuf;

	INT ret;

	ret = WSASend(m_hSock, &m_wsaSendBuf, 1, &sentbytes, 0, static_cast<OVERLAPPED*>(m_vActs[ACT_TYPE::ACT_SEND]), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			g_pLog->myWprintf("WSASend() Error!!!");

			return false;
		}
	}

	g_pLog->myWprintf("%d bytes were sent !\n", sentbytes);

	return true;
}