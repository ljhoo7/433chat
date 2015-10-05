#pragma once

#include "stdafx.h"

class TcpListenSocket
{

public:
	TcpListenSocket()
	{
		BacklogSize_ = 0;
	}

	void Init(WORD port, int backlogsize = 0)
	{
		Socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		Addr_.sin_family = AF_INET;
		Addr_.sin_addr.s_addr = htonl(INADDR_ANY);
		Addr_.sin_port = htons(port);

		BacklogSize_ = backlogsize;
	}

	void Listen(Proactor* proactor)
	{
		proactor->Register(reinterpret_cast<HANDLE>(Socket_));

		bind(Socket_, (SOCKADDR*)&Addr_, sizeof(Addr_));
		listen(Socket_, BacklogSize_);
	}

	SOCKET GetSocket() const
	{
		return Socket_;
	}

public:
	SOCKET		Socket_;
	SOCKADDR_IN Addr_;

	int			BacklogSize_;
};