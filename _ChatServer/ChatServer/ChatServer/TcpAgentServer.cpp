#include "stdafx.h"

TcpAgentServer::TcpAgentServer()
{
	proactor_ = NULL;
	acceptor_ = NULL;
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
	acceptor_ = new Acceptor;
	receiver_ = new Receiver;
	sender_ = new Sender;
	disconnector_ = new Disconnector;

	// Proactor initialize
	proactor_->Init(threadPoolSize_);
	// Listen sock initialize
	listenSocket_.Init(port_);
	// Listen start
	listenSocket_.Listen(proactor_);
	// Acceptor initialize - Manage socket pool with Disconnector
	acceptor_->Init(&listenSocket_, proactor_);
	// Receiver initialize - Manager user Pool, data transmission, parsing data
	receiver_->Init(proactor_);
	// Disconnector initialize  - Manage socket pool with Acceptor
	disconnector_->Init(proactor_);
	// Sender initialize 
	sender_->Init(proactor_);

	// Create Socket pool 
	socket = new AgentSocket(chatServer->serverNum);
	socket->Init();
	socket->InitAct(proactor_, acceptor_, disconnector_, NULL, sender_, receiver_);

	acceptor_->Register(*socket, 0);
}