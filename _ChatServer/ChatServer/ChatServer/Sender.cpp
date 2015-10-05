
#include "stdafx.h"

void Sender::ProcEvent(Act* act, DWORD bytes_transferred)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	tcpsocket.SendProcess(false, act, bytes_transferred);
	//printf("...Sender (%d byte) s(%d)\n", bytes_transferred, tcpsocket.GetSocket() );
}

void Sender::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	printf("...에러처리 Sender s(%d) err(%d)\n", tcpsocket.GetSocket(), error);

	tcpsocket.SendProcess(true, act, error);
}

void Sender::Init(Proactor* proactor)
{
	Proactor_ = proactor;
}