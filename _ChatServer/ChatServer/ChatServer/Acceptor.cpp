
#include "stdafx.h"

void Acceptor::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);
	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	assert(proactor_);

	//printf("...Acceptor s(%d)\n", tcpsocket.GetSocket());

	proactor_->Register((HANDLE)(tcpsocket.socket_));

	tcpsocket.AcceptProcess(false, act, bytes_transferred);
}

void Acceptor::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.tcpSocket_);

	TcpSocket& tcpsocket = *tcpact.tcpSocket_;

	printf("...에러처리 Acceptor s(%d) err(%d)\n", tcpsocket.socket_, error);

	tcpsocket.AcceptProcess(true, act, error);
}

void Acceptor::Init(TcpListenSocket* tcplistensocket, Proactor* proactor)
{
	tcpListenSocket_ = tcplistensocket;
	proactor_ = proactor;
}

void Acceptor::Register(TcpSocket& acceptsocket)
{
	DWORD byte_transferred;

	BOOL ret = AcceptEx(
		tcpListenSocket_->socket_,
		acceptsocket.socket_,
		acceptsocket.acceptBuf_,
		0,
		sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16,
		&byte_transferred,
		static_cast<OVERLAPPED*>(&acceptsocket.act_[TcpSocket::ACT_ACCEPT])
		);

	int error = WSAGetLastError();

	if (ret == FALSE && error != ERROR_IO_PENDING)
	{
		printf("AcceptEx Error!!! s(%d), err(%d)\n", acceptsocket.socket_, error);
	}
}
