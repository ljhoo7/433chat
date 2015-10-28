
#include "stdafx.h"

void Sender::ProcEvent(Act* act, DWORD bytes_transferred)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	tcpsocket.SendProcess(false, act, bytes_transferred);
	//PRINT("...Sender (%d byte) s(%d)\n", bytes_transferred, tcpsocket.GetSocket() );
}

void Sender::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	PRINT("Sender s(%d) err(%d)\n", tcpsocket.socket_, error);

	tcpsocket.SendProcess(true, act, error);
}

void Sender::Init(Proactor* proactor)
{
	proactor_ = proactor;
}