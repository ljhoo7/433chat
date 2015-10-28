#pragma once

struct ServerInfo{
	unsigned long ip;
	unsigned short inter_port;
	unsigned short client_port;

	boolean isWeb;
};

class ChatServer{

public:
	WORD agentPort;

	int identifierSeed;

	CRoomManager roomManager;
	CLogicHandle logicHandle;

	TcpClientServer* clientServer;
	TcpInterServer* interServer;
	TcpAgentServer* agentServer;
	
	CLogWriter *logWriter;
	int serverNum;
	std::vector<ServerInfo> serverInfo;	

//	int connG[MAXSERVER][MAXSERVER];
//	bool isVisit[MAXSERVER];

	bool isEnd;
	int DecreaseConnectServerAndCnt();
	int heartbeatTime;

public:
	ChatServer(int serverNum, WORD port);
	void Init();
	void Start();
	~ChatServer();

	void Process();

	void AddUser(CPlayer* player);
	void DeleteUser(CPlayer* player);
	int GetUserCnt();
	int GetUserCnt(int serverNum);
	CPlayer* FindUser(SOCKET socket, int serverNum);
	void RemoveOtherServerUsers(int serverNum);
	bool EnterOtherServerUsers();

	int GetServerNum(unsigned int ip, unsigned short port);

	void EndServer();
	void TotalUserInfo(char *buf, int *position, unsigned short *userCnt);

private:
	bool isCycle(int i, int parent);
	void DFS(int i);

	CRITICAL_SECTION connectServerLock;
	int connectServerCnt;


private:
	std::list<CPlayer*> users;
	CRITICAL_SECTION userLock;
};
