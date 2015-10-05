
#include "stdafx.h"

void Acceptor::ProcEvent(Act* act, DWORD bytes_transferred)
{
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);
	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	assert(Proactor_);

	//printf("...Acceptor s(%d)\n", tcpsocket.GetSocket());

	Proactor_->Register((HANDLE)(tcpsocket.GetSocket()));

	tcpsocket.Recv();
}

void Acceptor::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	printf("...����ó�� Acceptor s(%d) err(%d)\n", tcpsocket.GetSocket(), error);
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
		TcpListenSocket_->GetSocket(),
		acceptsocket.GetSocket(),
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
		printf("AcceptEx Error!!! s(%d), err(%d)\n", acceptsocket.GetSocket(), error);
	}
}
