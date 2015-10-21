#include "stdafx.h"

TcpAgentServer::TcpAgentServer()
{
	proactor_ = NULL;
	receiver_ = NULL;
	sender_ = NULL;
	disconnector_ = NULL;

	port_ = 0;
	threadPoolSize_ = 0;
}

TcpAgentServer::TcpAgentServer(WORD Port, int ThreadPoolSize)
{
	TcpAgentServer();
	port_ = Port;
	threadPoolSize_ = ThreadPoolSize;
	
}

void TcpAgentServer::Start()
{
	proactor_ = new Proactor;
	receiver_ = new Receiver;
	sender_ = new Sender;
	disconnector_ = new Disconnector;
	connector_ = new Connector;

	// Proactor initialize
	proactor_->Init(threadPoolSize_);
	connector_->Init(proactor_);
	// Receiver initialize - Manager user Pool, data transmission, parsing data
	receiver_->Init(proactor_);
	// Disconnector initialize  - Manage socket pool with Acceptor
	disconnector_->Init(proactor_);
	// Sender initialize 
	sender_->Init(proactor_);

	CreateConnectSocket();
}

void TcpAgentServer::CreateConnectSocket(){
	socket = new AgentSocket(chatServer->serverNum);
	socket->Init();
	socket->InitAct(proactor_, NULL, disconnector_, connector_, sender_, receiver_);

	socket->Bind(true);

	proactor_->Register((HANDLE)socket->socket_);
}

void TcpAgentServer::Connect(char* ip, WORD port){
	socket->Connect(inet_addr(ip), port);
}