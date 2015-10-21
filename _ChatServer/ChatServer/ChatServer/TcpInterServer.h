#pragma once

class TcpInterServer : public WinSockBase
{
public:
	TcpInterServer();
	TcpInterServer(WORD Port, int ThreadPoolSize, int socketPoolSize);
	void Start();
	void ConnectSocketCreate();

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;
	Connector*		connector_;

	TcpListenSocket	listenSocket_;

public:
	std::vector<InterSocket *> sockets; // 0: connection

	void Connect(int num);
	void Disconnect(int num);
	void SendAll(char* buf, int size);
	void SendWithoutOne(int serverNum, char* buf, int size);
	InterSocket* GetSocketWithNum(int serverNum);

/*public:
	std::thread heartThread;
	void heartbeatCheck();*/

private:
	WORD			port_;
	int				threadPoolSize_;
	int				socketPoolSize_;
};