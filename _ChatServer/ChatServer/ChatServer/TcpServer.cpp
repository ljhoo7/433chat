
#include "stdafx.h"

TcpServer::TcpServer()
{
	Proactor_ = NULL;
	Acceptor_ = NULL;
	Receiver_ = NULL;
	Sender_ = NULL;
	Disconnector_ = NULL;

	Port_ = 10000;
	ThreadPoolSize_ = 16;
	SocketPoolSize_ = 1000;
}


void TcpServer::Start()
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
		TcpSocket* socket = new TcpSocket;
		socket->Init();
		socket->InitAct(Proactor_, Acceptor_, Disconnector_, Sender_, Receiver_);

		Acceptor_->Register(*socket);
	}

}