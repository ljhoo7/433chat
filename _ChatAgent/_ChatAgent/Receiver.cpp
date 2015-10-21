#include "stdafx.h"


CReceiver::CReceiver(Proactor *p_pProactor)
{
	mProactor = p_pProactor;

}


CReceiver::~CReceiver()
{
}

void CReceiver::ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes)
{
	assert(dynamic_cast<TcpAct*>(p_pAct));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(p_pAct);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	//PRINTF("...에러처리 Receiver s(%d) err(%d)\n", tcpsocket.GetSocket(), error );

	tcpsocket.RecvProcess(true, p_pAct, p_dwTransferredBytes);



	
}
void CReceiver::ProcError(Act *p_pAct, DWORD p_dwError)
{

}
