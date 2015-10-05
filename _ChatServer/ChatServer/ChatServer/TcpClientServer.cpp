
#include "stdafx.h"

TcpClientServer::TcpClientServer()
{
	Proactor_ = NULL;
	Acceptor_ = NULL;
	Receiver_ = NULL;
	Sender_ = NULL;
	Disconnector_ = NULL;

	Port_ = 0;
	ThreadPoolSize_ = 0;
	SocketPoolSize_ = 0;
}

TcpClientServer::TcpClientServer(WORD Port, int ThreadPoolSize, int SocketPoolSize)
{
	TcpClientServer();
	Port_ = Port;
	ThreadPoolSize_ = ThreadPoolSize;
	SocketPoolSize_ = SocketPoolSize;
}

void TcpClientServer::Start()
{
	Proactor_ = new Proactor;
	Acceptor_ = new Acceptor;
	Receiver_ = new Receiver;
	Sender_ = new Sender;
	Disconnector_ = new Disconnector;

	// Proactor initialize
	Proactor_->Init(ThreadPoolSize_);
	// Listen sock initialize
	ListenSocket_.Init(Port_);
	// Listen start
	ListenSocket_.Listen(Proactor_);
	// Acceptor initialize - Manage socket pool with Disconnector
	Acceptor_->Init(&ListenSocket_, Proactor_);
	// Receiver initialize - Manager user Pool, data transmission, parsing data
	Receiver_->Init(Proactor_);
	// Disconnector initialize  - Manage socket pool with Acceptor
	Disconnector_->Init(Proactor_);
	// Sender initialize 
	Sender_->Init(Proactor_);

	// Create Socket pool 
	for (int i = 0; i<SocketPoolSize_; i++)
	{
		CPlayer* socket = new CPlayer(true);
		socket->Init();
		socket->InitAct(Proactor_, Acceptor_, Disconnector_, NULL, Sender_, Receiver_);

		Acceptor_->Register(*socket);
	}
}