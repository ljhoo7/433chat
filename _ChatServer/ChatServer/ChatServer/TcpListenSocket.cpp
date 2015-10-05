
#include "stdafx.h"

TcpListenSocket::TcpListenSocket()
{
	BacklogSize_ = 0;
}

void TcpListenSocket::Init(WORD port, int backlogsize)
{
	Socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	Addr_.sin_family = AF_INET;
	Addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	Addr_.sin_port = htons(port);

	BacklogSize_ = backlogsize;
}

void TcpListenSocket::Listen(Proactor* proactor)
{
	proactor->Register(reinterpret_cast<HANDLE>(Socket_));

	bind(Socket_, (SOCKADDR*)&Addr_, sizeof(Addr_));
	listen(Socket_, BacklogSize_);
}

SOCKET TcpListenSocket::GetSocket() const
{
	return Socket_;
}