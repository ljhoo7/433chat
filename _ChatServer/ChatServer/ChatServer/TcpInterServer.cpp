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
	TcpInterServer();
	port_ = Port;
	threadPoolSize_ = ThreadPoolSize;
	socketPoolSize_ = socketPoolSize;
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

	InterSocket * socket = new InterSocket(this, true);
	socket->Init();
	socket->InitAct(proactor_, NULL, disconnector_, connector_, sender_, receiver_);

	socket->Bind(true);

	proactor_->Register((HANDLE)socket->socket_);
	sockets.push_back(socket);
	PRINTF("InterConnectSocket start....\n");

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
		sockets.push_back(socket);
	}
	PRINTF("InterSocket start....\n");
}

void TcpInterServer::ConnectSocketCreate(){
	InterSocket * socket = new InterSocket(this, true);
	socket->Init();
	socket->InitAct(proactor_, NULL, disconnector_, connector_, sender_, receiver_);

	socket->Bind(true);

	proactor_->Register((HANDLE)socket->socket_);
	sockets[0] = socket;
}

void TcpInterServer::Connect(int num){
	ServerInfo serverInfo = chatServer->serverInfo[num];

	sockets[0]->Connect(serverInfo.ip, serverInfo.inter_port, num);
}

void TcpInterServer::Disconnect(int num){
	InterSocket* socket = GetSocketWithNum(num);
	if (socket == NULL){
		PRINTF("wrong server number(disconnect)\n");
		return;
	}
	socket->Disconnect();
}

void TcpInterServer::SendAll(char* buf, int size){
	for (unsigned int i = 0; i < sockets.size(); i++){
		if (sockets[i]->serverNum != -1){
			sockets[i]->Send(buf, size);
		}
	}
}

void TcpInterServer::SendWithoutOne(int serverNum, char* buf, int size){
	for (unsigned int i = 0; i < sockets.size(); i++){
		if (sockets[i]->serverNum != -1 && sockets[i]->serverNum != serverNum){
			sockets[i]->Send(buf, size);
		}
	}
}

InterSocket* TcpInterServer::GetSocketWithNum(int serverNum){
	for (unsigned int i = 0; i < sockets.size(); i++){
		if (sockets[i]->serverNum == serverNum){
			return sockets[i];
		}
	}
	return NULL;
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