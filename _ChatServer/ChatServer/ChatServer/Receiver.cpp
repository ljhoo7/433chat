
#include "stdafx.h"

void Receiver::ProcEvent(Act* act, DWORD bytes_transferred)
{
	assert(dynamic_cast<TcpAct*>(act));
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);
	assert(tcpact.tcpSocket_);
	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	//PRINTF(L"...Receiver (%d byte) s(%d)\n", bytes_transferred, tcpsocket.GetSocket() );

	// passive ¿¬°á²÷±è
	tcpsocket.RecvProcess(false, act, bytes_transferred);

}

void Receiver::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	//PRINTF(L"...¿¡·¯Ã³¸® Receiver s(%d) err(%d)\n", tcpsocket.GetSocket(), error );

	tcpsocket.RecvProcess(true, act, error);

}

void Receiver::Init(Proactor* proactor)
{
	proactor_ = proactor;
}