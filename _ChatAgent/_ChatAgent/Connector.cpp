#include "stdafx.h"

extern mswsock_s g_cMswsock;
extern CLogWriter *g_pLog;


extern DWORD			gServerIP;
extern unsigned int		gServerPort;

extern ServerAgent      *g_pServerAgent;

CConnector::CConnector(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;
}


CConnector::~CConnector()
{
}

void CConnector::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(p_pAct);
	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;
	assert(m_pProactor);

	PRINTF("...Acceptor s(%d)\n", tcpsocket.socket_);

	tcpsocket.ConnProcess(false, p_pAct, p_dwTransferredBytes);

	/*if (t_sMySock.m_hSock == g_pServerAgent->m_pSock->m_hSock)
	{
	
		PRINTF("The Server Connection has been made successfully !\n");

		t_sMySock.Recv(t_sMySock.m_szReceiveBuf, HEADER_SIZE);
	
	}
	else
	{

		PRINTF("The Monitoring Server Connection has been made successfully !\n");
	
	}*/
}

void CConnector::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}