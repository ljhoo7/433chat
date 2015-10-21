#include "stdafx.h"

extern CLogWriter	*g_pLog;
extern ServerAgent  *g_pServerAgent;

CReceiver::CReceiver(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;

}


CReceiver::~CReceiver()
{
}

void CReceiver::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	assert(dynamic_cast<TcpAct*>(p_pAct));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(p_pAct);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	//PRINTF("...에러처리 Receiver s(%d) err(%d)\n", tcpsocket.GetSocket(), error );

	tcpsocket.RecvProcess(true, p_pAct, p_dwTransferredBytes);



	
}
void CReceiver::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}
