#pragma once

#include "stdafx.h"

class TcpClientServer : public WinSockBase
{
public:
	TcpClientServer();
	TcpClientServer(WORD Port, int ThreadPoolSize, int SocketPoolSize);
	void Start();

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;

	TcpListenSocket	listenSocket_;

private:
	WORD			port_;
	int				threadPoolSize_;
	int				socketPoolSize_;

};