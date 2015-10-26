#include "stdafx.h"

extern ChatServer* chatServer;

TcpInterServer::TcpInterServer()
{
	proactor_ = NULL;
	acceptor_ = NULL;
	receiver_ = NULL;
	sender_ = NULL;
	disconnector_ = NULL;
	connector_ = NULL;
	port_ = 0;
	threadPoolSize_ = 0;
}

TcpInterServer::TcpInterServer(WORD Port, int ThreadPoolSize, int socketPoolSize)
{
	proactor_ = NULL;
	acceptor_ = NULL;
	receiver_ = NULL;
	sender_ = NULL;
	disconnector_ = NULL;
	connector_ = NULL;
	port_ = 0;
	threadPoolSize_ = 0;
	InitializeCriticalSection(&socketLock);
	InitializeCriticalSection(&connectSocketLock);

	port_ = Port;
	threadPoolSize_ = ThreadPoolSize;
	socketPoolSize_ = socketPoolSize;
}

TcpInterServer::~TcpInterServer()
{
	DeleteCriticalSection(&socketLock);
	DeleteCriticalSection(&connectSocketLock);
}

void TcpInterServer::Start()
{
	proactor_ = new Proactor;
	acceptor_ = new Acceptor;
	receiver_ = new Receiver;
	sender_ = new Sender;
	disconnector_ = new Disconnector;
	connector_ = new Connector;

	// Proactor initialize
	proactor_->Init(threadPoolSize_);
	
	// Receiver initialize - Manager user Pool, data transmission, parsing data
	receiver_->Init(proactor_);
	// Disconnector initialize  - Manage socket pool with Acceptor
	disconnector_->Init(proactor_);
	// Sender initialize 
	sender_->Init(proactor_);

	

	connector_->Init(proactor_);

	InterSocket * socket;

	// Listen sock initialize
	listenSocket_.Init(port_);
	// Listen start
	listenSocket_.Listen(proactor_);
	// Acceptor initialize - Manage socket pool with Disconnector
	acceptor_->Init(&listenSocket_, proactor_);

	for (int i = 0; i < socketPoolSize_; i++)
	{
		socket = new InterSocket(this, false);
		socket->Init();
		socket->InitAct(proactor_, acceptor_, disconnector_, NULL, sender_, receiver_);
		acceptor_->Register(*socket, sizeof(int));
		//sockets.push_back(socket);
	}
	PRINTF("InterSocket start....\n");
}

InterSocket* TcpInterServer::ConnectSocketCreate(){
	InterSocket * socket = new InterSocket(this, true);
	socket->Init();
	socket->InitAct(proactor_, NULL, disconnector_, connector_, sender_, receiver_);

	socket->Bind(true);

	proactor_->Register((HANDLE)socket->socket_);
	return socket;
}

void TcpInterServer::Connect(int num){
	ServerInfo serverInfo = chatServer->serverInfo[num];
	ConnectSocketCreate()->Connect(serverInfo.ip, serverInfo.inter_port, num);
}

void TcpInterServer::Disconnect(int num){
	InterSocket* socket = GetSocketWithNum(num);
	if (socket == NULL){
		socket->Disconnect();
		return;
	}

	socket = GetConnectSocketWithNum(num);
	if (socket == NULL){
		socket->Disconnect();
		return;
	}
}

int TcpInterServer::ServerCnt(){
	int ret = 0;
	EnterCriticalSection(&socketLock);
	ret += sockets.size();
	LeaveCriticalSection(&socketLock);

	EnterCriticalSection(&connectSocketLock);
	ret += connectSockets.size();
	LeaveCriticalSection(&connectSocketLock);

	return ret;
}

void TcpInterServer::SendAll(char* buf, int size){
	EnterCriticalSection(&socketLock);
	for (unsigned int i = 0; i < sockets.size(); i++){
		if (sockets[i]->serverNum != -1){
			sockets[i]->Send(buf, size);
		}
	}
	LeaveCriticalSection(&socketLock);

	EnterCriticalSection(&connectSocketLock);
	for (unsigned int i = 0; i < connectSockets.size(); i++){
		if (connectSockets[i]->serverNum != -1){
			connectSockets[i]->Send(buf, size);
		}
	}
	LeaveCriticalSection(&connectSocketLock);
}

InterSocket* TcpInterServer::GetSocketWithNum(int serverNum){
	EnterCriticalSection(&socketLock);
	InterSocket* ret = NULL;
	for (unsigned int i = 0; i < sockets.size(); i++){
		if (sockets[i]->serverNum == serverNum){
			ret=  sockets[i];
			break;
		}
	}
	LeaveCriticalSection(&socketLock);
	return ret;
}

InterSocket* TcpInterServer::GetConnectSocketWithNum(int serverNum){
	EnterCriticalSection(&connectSocketLock);
	InterSocket* ret = NULL;
	for (unsigned int i = 0; i < connectSockets.size(); i++){
		if (sockets[i]->serverNum == serverNum){
			ret = sockets[i];
		}
	}
	LeaveCriticalSection(&connectSocketLock);
	return ret;
}

void TcpInterServer::AddSocket(InterSocket* socket){
	EnterCriticalSection(&socketLock);
	sockets.push_back(socket);
	LeaveCriticalSection(&socketLock);
}

void TcpInterServer::AddConnectSocket(InterSocket* socket){
	EnterCriticalSection(&connectSocketLock);
	connectSockets.push_back(socket);
	LeaveCriticalSection(&connectSocketLock);
}

void TcpInterServer::DeleteSocket(InterSocket* socket){
	EnterCriticalSection(&socketLock);
	for (std::vector<InterSocket *>::iterator iter = sockets.begin(); iter != sockets.end();){
		if ((*iter) == socket){
			iter = sockets.erase(iter);
			break;
		}
		else iter++;
	}
	LeaveCriticalSection(&socketLock);
}

void TcpInterServer::DeleteConnectSocket(InterSocket* socket){
	EnterCriticalSection(&connectSocketLock);
	for (std::vector<InterSocket *>::iterator iter = connectSockets.begin(); iter != connectSockets.end();){
		if ((*iter) == socket){
			iter = connectSockets.erase(iter);
			break;
		}
		else iter++;
	}
	LeaveCriticalSection(&connectSocketLock);
}

void TcpInterServer::ShowConnectServerList(){
	PRINTF("\n--------------------------server info----------------------\n");
	EnterCriticalSection(&socketLock);
	for (std::vector<InterSocket *>::iterator iter = sockets.begin(); iter != sockets.end(); iter++){
		PRINTF("%d server (accept)\n", (*iter)->serverNum);
	}
	LeaveCriticalSection(&socketLock);

	EnterCriticalSection(&connectSocketLock);
	for (std::vector<InterSocket *>::iterator iter = connectSockets.begin(); iter != connectSockets.end(); iter++){
		PRINTF("%d server (connect)\n", (*iter)->serverNum);
	}
	LeaveCriticalSection(&connectSocketLock);
	PRINTF("-----------------------------------------------------------\n\n");
}

void TcpInterServer::GetServerNums(std::vector<int>& list){
	EnterCriticalSection(&socketLock);
	for (std::vector<InterSocket *>::iterator iter = sockets.begin(); iter != sockets.end(); iter++){
		if (!chatServer->serverInfo[(*iter)->serverNum].isWeb){
			list.push_back((*iter)->serverNum);
		}
	}
	LeaveCriticalSection(&socketLock);

	EnterCriticalSection(&connectSocketLock);
	for (std::vector<InterSocket *>::iterator iter = connectSockets.begin(); iter != connectSockets.end(); iter++){
		if (!chatServer->serverInfo[(*iter)->serverNum].isWeb){
			list.push_back((*iter)->serverNum);
		}
	}
	LeaveCriticalSection(&connectSocketLock);
}

void TcpInterServer::DisconnectAllServers(){
	if (this->ServerCnt() == 0){
		if (chatServer->agentServer->socket->isConnected){
			PRINTF("disconnect all interserver success!\n");
			PRINTF("starting agentserver disconnect...\n");
			chatServer->agentServer->socket->Disconnect();
		}
		else{
			PRINTF("all disconnect success!!\nend complete\n");
		}
	}

	EnterCriticalSection(&socketLock);
	for (std::vector<InterSocket *>::iterator iter = sockets.begin(); iter != sockets.end(); iter++){
		(*iter)->Disconnect();
	}
	LeaveCriticalSection(&socketLock);

	EnterCriticalSection(&connectSocketLock);
	for (std::vector<InterSocket *>::iterator iter = connectSockets.begin(); iter != connectSockets.end(); iter++){
		(*iter)->Disconnect();
	}
	LeaveCriticalSection(&connectSocketLock);
}



/*
void TcpInterServer::heartbeatCheck(){
	while (true)
	{
		char *buf = poolManager->Alloc()->buf;

		beatCheck = false;

		if (isUse){
			PRINTF("hearth check send!\n");
			ss_heartbeats* msg = (ss_heartbeats *)buf;
			msg->type = ssType::pkt_heartbeats;
			socket->Send((char *)msg, sizeof(ss_heartbeats));
			std::this_thread::sleep_for(std::chrono::seconds(5));
			if (beatCheck == false) break;
		}
		else{
			break;
		}
	}
	PRINTF("connection fail\n");
	if (isUse) socket->Disconnect();
}
*/