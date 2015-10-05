
#include "stdafx.h"

void Disconnector::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);
	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	//printf("...Disconnector s(%d)\n", tcpsocket.GetSocket());

	assert(tcpsocket.Acceptor_);

	// reuse socket
	tcpsocket.Reuse();
}

void Disconnector::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	printf("...에러처리 Disconnector s(%d) err(%d)\n", tcpsocket.GetSocket(), error);
}

void Disconnector::Init(Proactor* proactor)
{
	Proactor_ = proactor;
}