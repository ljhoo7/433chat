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
		info.isConnected = false;

		serverInfo.push_back(info);
	}

	inFile.close();

	logWriter = new CLogWriter(L"ServerLog.log", 2);

	interServer = new TcpInterServer(serverInfo[serverNum].inter_port, 10, 10);
	clientServer = new TcpClientServer(serverInfo[serverNum].client_port, 10, 3000);
}

void ChatServer::Start(){
	std::thread logic_thread(&CLogicHandle::Process, &logicHandle);
	clientServer->Start();
	interServer->Start();

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
					p->isMine, p->socket_, p->roomNum, p->nickname.c_str());
			}
			PRINTF("-----------------------------------------------------------\n");
		}
		if (input == "quit")
		{
			break;
		}
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

CPlayer* ChatServer::FindUser(SOCKET socket){
	EnterCriticalSection(&userLock);
	std::list<CPlayer*>::iterator iter;
	for (iter = users.begin(); iter != users.end(); iter++)
	{
		CPlayer* p = *(iter);
		if (!p->isMine && p->socket_ == socket)
		{
			LeaveCriticalSection(&userLock);
			return p;
		}
	}
	LeaveCriticalSection(&userLock);
	return NULL;
}

int ChatServer::GetUserCnt(){
	int ret;
	EnterCriticalSection(&userLock);
	ret = users.size();
	LeaveCriticalSection(&userLock);

	return ret;
}

bool ChatServer::DeleteOtherServerUsers(int serverNum){
	bool check = true;
	EnterCriticalSection(&userLock);
	std::list<CPlayer*>::iterator iter;
	for (iter = users.begin(); iter != users.end(); iter++)
	{
		CPlayer* p = (*iter);
		if (!p->isMine && p->roomNum != -1)
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