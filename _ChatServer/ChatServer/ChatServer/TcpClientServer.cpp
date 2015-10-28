
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

	InitializeCriticalSectionAndSpinCount(&playerlistLock, 4000);
}

TcpClientServer::~TcpClientServer()
{
	DeleteCriticalSection(&playerlistLock);
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
	EnterCriticalSection(&playerlistLock);
	playerlist.push_back(player);
	LeaveCriticalSection(&playerlistLock);
}

int TcpClientServer::DeleteUserAndCnt(CPlayer* player)
{
	int ret = 0;
	EnterCriticalSection(&playerlistLock);
	playerlist.remove(player);
	ret = playerlist.size();
	LeaveCriticalSection(&playerlistLock);

	return ret;
}

void TcpClientServer::EscapingAllUsers()
{
	EnterCriticalSection(&playerlistLock);
	int i = 0;
	for (std::list<CPlayer*>::iterator iter = playerlist.begin();
		iter != playerlist.end(); ++iter)
	{
		(*iter)->EscapePlayer(true, i);
		i++;
	}
	LeaveCriticalSection(&playerlistLock);
}



void TcpClientServer::HeartbeatCheck()
{
	while (!chatServer->isEnd){
		EnterCriticalSection(&playerlistLock);
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
		LeaveCriticalSection(&playerlistLock);

		std::this_thread::sleep_for(std::chrono::seconds(chatServer->heartbeatTime));
		if (chatServer->isEnd) return;

		EnterCriticalSection(&playerlistLock);
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
		LeaveCriticalSection(&playerlistLock);
	}
}

void TcpClientServer::TotalUserInfo(sag_total_user_info* msg, int* size_){
	msg->type = sag_pkt_type::pt_total_user_info;

	EnterCriticalSection(&playerlistLock);


	*size_ = sizeof(msg->type) + sizeof(msg->userCnt);
	int i = 0;
	std::list<CPlayer*>::iterator iter;
	for (iter = playerlist.begin(); iter != playerlist.end(); iter++)
	{

		CPlayer *p = (*iter);
		msg->userInfoList[i].roomNum = p->roomNum;
		memcpy(msg->userInfoList[i].userName, p->nickname.c_str(), sizeof(p->nickname));
		msg->userInfoList[i].userSocket = (int)p->socket_;

		i++;

		*size_ += sizeof(SAGUserInfo);
	}
	msg->userCnt = i;
	LeaveCriticalSection(&playerlistLock);

}

void TcpClientServer::UserOut(int clientSocket){
	
	for (std::list<CPlayer*>::iterator iter = playerlist.begin();
		iter != playerlist.end(); ++iter)
	{
		if ((*iter)->socket_ == clientSocket)
		{
			t_user_out tmpUserOut;
			tmpUserOut.type = pkt_type::pt_user_out_client;
			tmpUserOut.client_socket = clientSocket;
			(*iter)->Send((char*)&tmpUserOut, sizeof(t_user_out));
		}
	}
}