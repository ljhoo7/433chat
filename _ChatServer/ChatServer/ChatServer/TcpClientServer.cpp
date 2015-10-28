
#include "stdafx.h"

TcpClientServer::TcpClientServer()
{
	proactor_ = NULL;
	acceptor_ = NULL;
	receiver_ = NULL;
	sender_ = NULL;
	disconnector_ = NULL;

	port_ = 0;
	threadPoolSize_ = 0;
	socketPoolSize_ = 0;
}

TcpClientServer::TcpClientServer(WORD Port, int ThreadPoolSize, int SocketPoolSize)
{
	//TcpClientServer();

	proactor_ = NULL;
	acceptor_ = NULL;
	receiver_ = NULL;
	sender_ = NULL;
	disconnector_ = NULL;

	port_ = Port;
	threadPoolSize_ = ThreadPoolSize;
	socketPoolSize_ = SocketPoolSize;

	InitializeCriticalSectionAndSpinCount(&playerLock, 4000);
}

TcpClientServer::~TcpClientServer()
{
	DeleteCriticalSection(&playerLock);
}

void TcpClientServer::Start()
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
	for (int i = 0; i<socketPoolSize_; i++)
	{
		CPlayer* socket = new CPlayer(chatServer->serverNum);
		socket->Init();
		socket->InitAct(proactor_, acceptor_, disconnector_, NULL, sender_, receiver_);

		acceptor_->Register(*socket, 0);
	}

	heartbeatThread = std::thread(&TcpClientServer::HeartbeatCheck, this);
}

void TcpClientServer::AddUser(CPlayer* player)
{
	EnterCriticalSection(&playerLock);
	playerlist.push_back(player);
	LeaveCriticalSection(&playerLock);
}

int TcpClientServer::DeleteUserAndCnt(CPlayer* player)
{
	int ret = 0;
	EnterCriticalSection(&playerLock);
	playerlist.remove(player);
	ret = playerlist.size();
	LeaveCriticalSection(&playerLock);

	return ret;
}

void TcpClientServer::EscapingAllUsers()
{
	EnterCriticalSection(&playerLock);
	int i = 0;
	for (std::list<CPlayer*>::iterator iter = playerlist.begin();
		iter != playerlist.end(); ++iter)
	{
		(*iter)->EscapePlayer(true, i);
		i++;
	}
	LeaveCriticalSection(&playerLock);
}



void TcpClientServer::HeartbeatCheck()
{
	while (!chatServer->isEnd){
		EnterCriticalSection(&playerLock);
#ifdef HEARTBEAT
		PRINT("[ClientServer] **heartbeat check / player cnt : %d\n", playerlist.size());
#endif
		for (std::list<CPlayer*>::iterator iter = playerlist.begin();
			iter != playerlist.end(); ++iter)
		{
			(*iter)->beatCheck = false;

			t_health_check msg;
			msg.type = pkt_type::pt_cs_health_check;
			(*iter)->Send((char *)&msg, sizeof(msg));
		}
		LeaveCriticalSection(&playerLock);

		std::this_thread::sleep_for(std::chrono::seconds(chatServer->heartbeatTime));
		if (chatServer->isEnd) return;

		EnterCriticalSection(&playerLock);
		int cnt = 0;
		for (std::list<CPlayer*>::iterator iter = playerlist.begin();
			iter != playerlist.end(); ++iter)
		{
			if ((*iter)->beatCheck == false){
				(*iter)->Disconnect();
				cnt++;
			}
		}
#ifdef HEARTBEAT
		PRINT("[ClientServer] **heartbeat check clear / player cnt : %d\n", playerlist.size()-cnt);
#endif
		LeaveCriticalSection(&playerLock);
	}
}