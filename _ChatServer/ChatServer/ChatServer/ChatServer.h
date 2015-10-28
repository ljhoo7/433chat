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

//	int connG[MAXSERVER][MAXSERVER];
//	bool isVisit[MAXSERVER];

	bool isEnd;

	int connectServerCnt;

	int DecreaseConnectServerAndCnt();
	CRITICAL_SECTION connectServerLock;

public:
	ChatServer(int serverNum, WORD port);
	void Init();
	void Start();
	~ChatServer();

	void Process();

	void AddUser(CPlayer* player);
	void DeleteUser(CPlayer* player);
	int DeleteUserAndCnt(CPlayer* player);
	int GetUserCnt();
	int GetUserCnt(int serverNum);
	CPlayer* FindUser(SOCKET socket, int serverNum);
	void RemoveOtherServerUsers(int serverNum);
	bool EnterOtherServerUsers(int serverNum);

	int GetServerNum(unsigned int ip, unsigned short port);

	void EscapingAllUsers();
	void EndServer();

private:
	bool isCycle(int i, int parent);
	void DFS(int i);

};
