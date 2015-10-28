#pragma once

class TcpInterServer : public WinSockBase
{
public:
	TcpInterServer();
	TcpInterServer(WORD Port, int ThreadPoolSize, int socketPoolSize);
	~TcpInterServer();
	void Start();
	InterSocket* ConnectSocketCreate();

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;
	Connector*		connector_;

	TcpListenSocket	listenSocket_;


public:
	void Connect(int num);
	void Disconnect(int num);
	void SendAll(char* buf, int size);
	//void SendWithoutOne(int serverNum, char* buf, int size);
	InterSocket* GetSocketWithNum(int serverNum);
	InterSocket* GetConnectSocketWithNum(int serverNum);

	void AddSocket(InterSocket* socket, bool isConnect);
	void DeleteSocket(InterSocket* socket);
	int DeleteSocketAndCnt(InterSocket* socket);

	void ShowConnectServerList();
	int ServerCnt();
	void GetServerNums(std::vector<int>& list);
	void DisconnectAllServers();

/*public:
	std::thread heartThread;
	void heartbeatCheck();*/

private:
	WORD			port_;
	int				threadPoolSize_;
	int				socketPoolSize_;


private:
	std::vector<InterSocket *> sockets; // connected socket
	std::vector<InterSocket *> connectSockets; // connecting socket
	CRITICAL_SECTION socketLock;
};