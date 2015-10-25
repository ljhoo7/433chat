#include "stdafx.h"

Connector::Connector(Proactor *p_pProactor)
{
	mProactor = p_pProactor;
}


Connector::~Connector()
{
}

void Connector::ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(p_pAct);
	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;
	assert(mProactor);

	PRINTF("...Acceptor s(%d)\n", tcpsocket.socket_);

	tcpsocket.ConnProcess(false, p_pAct, p_dwTransferredBytes);

}

void Connector::ProcError(Act *p_pAct, DWORD p_dwError)
{

}