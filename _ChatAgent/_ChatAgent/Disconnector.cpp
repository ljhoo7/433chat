#include "stdafx.h"

Disconnector::Disconnector(Proactor *p_pProactor)
{
	mProactor = p_pProactor;
}


Disconnector::~Disconnector()
{
}

void Disconnector::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);
	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	PRINTF("...Disconnector s(%d)\n", tcpsocket.socket_);

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

	PRINTF("...에러처리 Disconnector s(%d) err(%d)\n", tcpsocket.socket_, error);

	tcpsocket.DisconnProcess(true, act, error);
}