#include "stdafx.h"
#include <fstream>
#include <iostream>

ChatServer::ChatServer(int serverNum){
	this->serverNum = serverNum;
	Init();
}

ChatServer::~ChatServer(){
	DeleteCriticalSection(&userLock);
}

void Tokenize(const std::string& str,
	std::vector<std::string>& tokens,
	const std::string& delimiters = " ")
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// 구분자가 아닌 첫 글자를 찾는다
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of(delimiters, pos);
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void ChatServer::Init(){
	InitializeCriticalSectionAndSpinCount(&userLock, 4000);

	identifierSeed = 0;

	std::ifstream inFile("ServerInfo.txt");

	while (!inFile.eof()){
		std::string temp;
		int port1;
		int port2;

		inFile >> temp >> port1 >> port2;


		ServerInfo info;
		info.ip = inet_addr(temp.c_str());
		info.inter_port = port1;
		info.client_port = port2;
		serverInfo.push_back(info);
	}

	inFile.close();

	logWriter = new CLogWriter(L"ServerLog.log", 2);

	interServer = new TcpInterServer(serverInfo[serverNum].inter_port, 10, 10);
	clientServer = new TcpClientServer(serverInfo[serverNum].client_port, 10, 3000);
	agentServer = new TcpAgentServer(serverInfo[serverNum].inter_port + 1, 10);

	for (int i = 0; i < MAXSERVER; i++){
		for (int j = 0; j < MAXSERVER; j++){
			connG[i][j] = 0;
		}
	}
}

void ChatServer::Start(){
	std::thread logic_thread(&CLogicHandle::Process, &logicHandle);
	clientServer->Start();
	interServer->Start();
	agentServer->Start();

	while (true)
	{
		std::string input;
		//ZeroMemory(temp, sizeof(temp));
		std::getline(std::cin, input);

		if (input == "connect")
		{
			printf("Enter server number : ");
			std::string temp;
			std::getline(std::cin, temp);
			unsigned int num = atoi(temp.c_str());

			if (num >= serverInfo.size()){
				printf("wrong input\n");
				continue;
			}
			interServer->Connect(num);
		}
		else if (input == "disconnect")
		{
			printf("Enter server number : ");
			std::string temp;
			std::getline(std::cin, temp);
			unsigned int num = atoi(temp.c_str());

			if (num >= serverInfo.size()){
				printf("wrong input\n");
				continue;
			}

			interServer->Disconnect(num);
		}

		if (input == "show")
		{
			std::list<CPlayer*>::iterator iter;
			PRINTF("\n--------------------------player info----------------------\n");
			for (iter = users.begin(); iter != users.end(); iter++)
			{
				CPlayer *p = (*iter);
				PRINTF("is Mine : %d, client socket : %d, room Num : %d, nickname : %s\n",
					p->serverNum, p->socket_, p->roomNum, p->nickname.c_str());
			}
			PRINTF("-----------------------------------------------------------\n");
		}
		if (input == "quit")
		{
			break;
		}
	}

	PRINTF("END?\n");
	// escaping all user to another server

	for (std::list<CPlayer*>::iterator iter = chatServer->users.begin();
		iter != chatServer->users.end(); ++iter)
	{
		t_escape_server tmpEscape;
		tmpEscape.type = pkt_type::pt_escape_server;

		int sernum = chatServer->serverNum;

		if ((*iter)->serverNum == chatServer->serverNum)
		{
			for (unsigned int k = 0; k < chatServer->serverInfo.size(); ++k)
			{
				if (chatServer->connG[sernum][k])
				{
					tmpEscape.dest_ip = chatServer->serverInfo[k].ip;
					tmpEscape.port = chatServer->serverInfo[k].client_port;

					(*iter)->Send((char*)&tmpEscape, sizeof(t_escape_server));

					break;
				}
			}
		}
	}

	/* disconnect interserver */

	for (unsigned int k = 0; k < chatServer->interServer->sockets.size(); ++k)
	{
		InterSocket* socket = chatServer->interServer->sockets[k];
		if (socket->isConnect) socket->Disconnect();
	}

	logic_thread.join();
}

void ChatServer::AddUser(CPlayer* player){
	EnterCriticalSection(&userLock);
	users.push_back(player);
	LeaveCriticalSection(&userLock);
}

void ChatServer::DeleteUser(CPlayer* player){
	EnterCriticalSection(&userLock);
	users.remove(player);
	LeaveCriticalSection(&userLock);
}

CPlayer* ChatServer::FindUser(SOCKET socket, int serverNum){
	EnterCriticalSection(&userLock);
	std::list<CPlayer*>::iterator iter;
	for (iter = users.begin(); iter != users.end(); iter++)
	{
		CPlayer* p = *(iter);
		if (p->serverNum == serverNum && p->socket_ == socket)
		{
			LeaveCriticalSection(&userLock);
			return p;
		}
	}
	LeaveCriticalSection(&userLock);
	return NULL;
}

int ChatServer::GetUserCnt(){
	int ret = 0;
	EnterCriticalSection(&userLock);
	ret = users.size();
	LeaveCriticalSection(&userLock);

	return ret;
}


void ChatServer::RemoveOtherServerUsers(int serverNum){
	EnterCriticalSection(&chatServer->userLock);
	std::list<CPlayer*>::iterator iter;
	for (iter = chatServer->users.begin(); iter != chatServer->users.end();)
	{
		if ((*iter)->serverNum == serverNum)
		{
			if ((*iter)->roomNum != -1)
			{
				chatServer->roomManager.LeaveRoom((*iter), (*iter)->roomNum);
			}
			PRINTF("delete other server's user : %d\n", (*iter)->socket_);
			iter = chatServer->users.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	LeaveCriticalSection(&chatServer->userLock);
}

bool ChatServer::EnterOtherServerUsers(int serverNum){
	bool check = true;
	EnterCriticalSection(&userLock);
	std::list<CPlayer*>::iterator iter;
	for (iter = users.begin(); iter != users.end(); iter++)
	{
		CPlayer* p = (*iter);
		if (p->serverNum==serverNum && p->roomNum != -1)
		{
			int ret = roomManager.EnterRoom(p, p->roomNum);
			if (ret != -1){
				//	disconnect();
				check = false;
				break;
			}
		}
	}
	LeaveCriticalSection(&userLock);

	return check;
}

int ChatServer::GetServerNum(unsigned int ip, unsigned short port){
	for (unsigned int i = 0; i < serverInfo.size(); i++){
		if (serverInfo[i].ip == ip && serverInfo[i].inter_port == port){
			return i;
		}
	}
	return -1;
}

bool ChatServer::isCycle(int i, int parent){
	isVisit[i] = true;

	for (unsigned int j = 0; j < serverInfo.size(); j++){
		if (connG[i][j] == 1){
			if (!isVisit[j]){
				if (isCycle(j, i)) return true;
			}
			else{
				if (j != parent) return true;
			}
		}
		
	}
	return false;

}

bool ChatServer::ConnectServer(int serverNum1, int serverNum2, bool check){
	if (serverNum1 == serverNum2) return false;
	connG[serverNum1][serverNum2] = 1;
	connG[serverNum2][serverNum1] = 1;

	if (!check) return true;

	for (unsigned int i = 0; i < serverInfo.size(); i++){
		isVisit[i] = false;
	}

	if (isCycle(0, -1)){
		connG[serverNum1][serverNum2] = 0;
		connG[serverNum2][serverNum1] = 0;
		return false;
	}
	return true;
}

void ChatServer::DFS(int i){
	isVisit[i] = true;

	for (unsigned int j = 0; j < serverInfo.size(); j++){
		if (connG[i][j] == 1){
			if (!isVisit[j]){
				DFS(j);
			}
		}
	}
}

void ChatServer::DisconnectServer(int serverNum1, int serverNum2){
	connG[serverNum1][serverNum2] = 0;
	connG[serverNum2][serverNum1] = 0;

	for (unsigned int i = 0; i < serverInfo.size(); i++){
		isVisit[i] = false;
	}

	DFS(serverNum);
	
	for (unsigned int i = 0; i < serverInfo.size(); i++){
		if (!isVisit[i]){
			RemoveOtherServerUsers(i);
		}
	}

}