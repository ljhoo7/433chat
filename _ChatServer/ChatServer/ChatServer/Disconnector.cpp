
#include "stdafx.h"

void Disconnector::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);
	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	//printf("...Disconnector s(%d)\n", tcpsocket.GetSocket());

	assert(tcpsocket.Disconnector_);

	tcpsocket.DisconnProcess(false, act, bytes_transferred);
	// reuse socket
	
}

void Disconnector::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	printf("...에러처리 Disconnector s(%d) err(%d)\n", tcpsocket.GetSocket(), error);

	tcpsocket.DisconnProcess(true, act, error);
}

void Disconnector::Init(Proactor* proactor)
{
	Proactor_ = proactor;
}