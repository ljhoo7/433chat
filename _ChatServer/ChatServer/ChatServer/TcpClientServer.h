#pragma once

#include "stdafx.h"

class TcpClientServer : public WinSockBase
{
public:
	TcpClientServer();
	TcpClientServer(WORD Port, int ThreadPoolSize, int SocketPoolSize);
	void Start();

public:
	Proactor*		Proactor_;
	Acceptor*		Acceptor_;
	Receiver*		Receiver_;
	Sender*			Sender_;
	Disconnector*	Disconnector_;

	TcpListenSocket	ListenSocket_;

private:
	WORD			Port_;
	int				ThreadPoolSize_;
	int				SocketPoolSize_;

};