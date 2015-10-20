#pragma once

class TcpAgentServer : public WinSockBase
{
public:
	TcpAgentServer();
	TcpAgentServer(WORD Port, int ThreadPoolSize);
	void Start();

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;

	TcpListenSocket	listenSocket_;

	AgentSocket* socket;

private:
	WORD			port_;
	int				threadPoolSize_;

};