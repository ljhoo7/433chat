#pragma once

class TcpClientServer : public WinSockBase
{
public:
	TcpClientServer();
	~TcpClientServer();
	TcpClientServer(WORD Port, int ThreadPoolSize, int SocketPoolSize);
	void Start();

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;

	TcpListenSocket	listenSocket_;

public:
	void AddUser(CPlayer* player);
	int DeleteUserAndCnt(CPlayer* player);
	void EscapingAllUsers();

	std::list<CPlayer*> playerlist;
	CRITICAL_SECTION playerLock;


private:
	WORD			port_;
	int				threadPoolSize_;
	int				socketPoolSize_;

public:
	std::thread heartbeatThread;
	void HeartbeatCheck();
};