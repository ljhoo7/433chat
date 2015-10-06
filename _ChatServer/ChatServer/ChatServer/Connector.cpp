
#include "stdafx.h"

void Connector::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);
	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	assert(Proactor_);

	//printf("...Acceptor s(%d)\n", tcpsocket.GetSocket());

	Proactor_->Register((HANDLE)(tcpsocket.Socket_));

	tcpsocket.ConnProcess(false, act, bytes_transferred);
}

void Connector::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	printf("...에러처리 Acceptor s(%d) err(%d)\n", tcpsocket.Socket_, error);

	tcpsocket.ConnProcess(true, act, error);
}

void Connector::Init(Proactor* proactor)
{
	Proactor_ = proactor;
}