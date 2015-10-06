#pragma once

#include "stdafx.h"

class TcpListenSocket
{

public:
	TcpListenSocket();
	void Init(WORD port, int backlogsize = 0);
	void Listen(Proactor* proactor);
	SOCKET GetSocket() const;

public:
	SOCKET		Socket_;
	SOCKADDR_IN Addr_;

	int			BacklogSize_;
};

