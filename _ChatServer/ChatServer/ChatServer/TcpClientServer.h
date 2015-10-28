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
	void TotalUserInfo(sag_total_user_info* msg, int* size);
	void UserOut(int clientSocket);

public:
	std::thread heartbeatThread;
	void HeartbeatCheck();

private:
	WORD			port_;
	int				threadPoolSize_;
	int				socketPoolSize_;

	std::list<CPlayer*> playerlist;
	CRITICAL_SECTION playerlistLock;


};