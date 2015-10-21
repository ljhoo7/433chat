#pragma once

class TcpAgentServer : public WinSockBase
{
public:
	TcpAgentServer();
	TcpAgentServer(WORD Port, int ThreadPoolSize);
	void Start();
	void CreateConnectSocket();
	void Connect(char* ip, WORD port);

public:
	Proactor*		proactor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;
	Connector*		connector_;

	AgentSocket* socket;

private:
	WORD			port_;
	int				threadPoolSize_;

};