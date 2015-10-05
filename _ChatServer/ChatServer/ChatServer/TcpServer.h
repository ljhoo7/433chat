#pragma once

class TcpServer : public WinSockBase
{
public:
	TcpServer();

public:
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