
#include "stdafx.h"

void Acceptor::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);
	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	assert(Proactor_);

	//printf("...Acceptor s(%d)\n", tcpsocket.GetSocket());

	Proactor_->Register((HANDLE)(tcpsocket.Socket_));

	tcpsocket.AcceptProcess(false, act, bytes_transferred);
}

void Acceptor::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	printf("...����ó�� Acceptor s(%d) err(%d)\n", tcpsocket.Socket_, error);

	tcpsocket.AcceptProcess(true, act, error);
}

void Acceptor::Init(TcpListenSocket* tcplistensocket, Proactor* proactor)
{
	TcpListenSocket_ = tcplistensocket;
	Proactor_ = proactor;
}

void Acceptor::Register(TcpSocket& acceptsocket)
{
	DWORD byte_transferred;

	BOOL ret = AcceptEx(
		TcpListenSocket_->Socket_,
		acceptsocket.Socket_,
		acceptsocket.AcceptBuf_,
		0,
		sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16,
		&byte_transferred,
		static_cast<OVERLAPPED*>(&acceptsocket.Act_[TcpSocket::ACT_ACCEPT])
		);

	int error = WSAGetLastError();

	if (ret == FALSE && error != ERROR_IO_PENDING)
	{
		printf("AcceptEx Error!!! s(%d), err(%d)\n", acceptsocket.Socket_, error);
	}
}
