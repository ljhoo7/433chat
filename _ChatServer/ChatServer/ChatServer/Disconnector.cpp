
#include "stdafx.h"

void Disconnector::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);
	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	//PRINTF(L"...Disconnector s(%d)\n", tcpsocket.GetSocket());

	assert(tcpsocket.disconnector_);

	tcpsocket.DisconnProcess(false, act, bytes_transferred);
	// reuse socket
	
}

void Disconnector::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	PRINTF(L"...에러처리 Disconnector s(%d) err(%d)\n", tcpsocket.socket_, error);

	tcpsocket.DisconnProcess(true, act, error);
}

void Disconnector::Init(Proactor* proactor)
{
	proactor_ = proactor;
}