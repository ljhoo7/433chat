#pragma once

struct ServerInfo{
	unsigned long ip;
	unsigned short inter_port;
	unsigned short client_port;
	bool isConnected;
};

class ChatServer{

public:
	int identifierSeed;
	std::list<CPlayer*> users;
	CRITICAL_SECTION userLock;

	CRoomManager roomManager;
	CLogicHandle logicHandle;

	TcpClientServer* clientServer;
	TcpInterServer* interServer;
	
	CLogWriter *logWriter;
	int serverNum;
	std::vector<ServerInfo> serverInfo;	

public:
	ChatServer(int serverNum);
	void Init();
	void Start();
	~ChatServer();

	void AddUser(CPlayer* player);
	void DeleteUser(CPlayer* player);
	int GetUserCnt();
	CPlayer* FindUser(SOCKET socket);
	bool DeleteOtherServerUsers(int serverNum);

	int GetServerNum(unsigned int ip, unsigned short port);

};
