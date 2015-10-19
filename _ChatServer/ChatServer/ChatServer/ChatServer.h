#pragma once

struct ServerInfo{
	unsigned long ip;
	unsigned short inter_port;
	unsigned short client_port;
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
	TcpAgentServer* agentServer;
	
	CLogWriter *logWriter;
	int serverNum;
	std::vector<ServerInfo> serverInfo;	

	int connG[MAXSERVER][MAXSERVER];
	bool isVisit[MAXSERVER];

public:
	ChatServer(int serverNum);
	void Init();
	void Start();
	~ChatServer();

	void AddUser(CPlayer* player);
	void DeleteUser(CPlayer* player);
	int GetUserCnt();
	CPlayer* FindUser(SOCKET socket, int serverNum);
	void RemoveOtherServerUsers(int serverNum);
	bool EnterOtherServerUsers(int serverNum);

	bool ConnectServer(int serverNum1, int serverNum2, bool check);
	void DisconnectServer(int serverNum1, int serverNum2);

	int GetServerNum(unsigned int ip, unsigned short port);

private:
	bool isCycle(int i, int parent);
	void DFS(int i);

};
