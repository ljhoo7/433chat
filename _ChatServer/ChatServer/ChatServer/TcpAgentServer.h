#pragma once

class TcpAgentServer : public WinSockBase
{
public:
	TcpAgentServer();
	TcpAgentServer(WORD Port, int ThreadPoolSize, int socketPoolSize);
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