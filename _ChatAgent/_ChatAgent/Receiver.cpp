#include "stdafx.h"


Receiver::Receiver(Proactor *p_pProactor)
{
	mProactor = p_pProactor;

}


Receiver::~Receiver()
{
}

void Receiver::ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes)
{
	assert(dynamic_cast<TcpAct*>(p_pAct));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(p_pAct);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	//PRINTF("...에러처리 Receiver s(%d) err(%d)\n", tcpsocket.GetSocket(), error );

	tcpsocket.RecvProcess(false, p_pAct, p_dwTransferredBytes);



	
}
void Receiver::ProcError(Act *p_pAct, DWORD p_dwError)
{
	assert(dynamic_cast<TcpAct*>(p_pAct));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(p_pAct);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	PRINTF("...Receiver err(%d)\n");

	tcpsocket.RecvProcess(true, p_pAct, 0);
}
