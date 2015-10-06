#include "stdafx.h"


extern CLogicHandle logicHandle;
extern CRoomManager roomManager;
extern std::list<CPlayer*> g_vPlayers;

TcpInterServer::TcpInterServer()
{
	Proactor_ = NULL;
	Acceptor_ = NULL;
	Receiver_ = NULL;
	Sender_ = NULL;
	Disconnector_ = NULL;
	Connector_ = NULL;
	isConnect = false;

	Port_ = 0;
	ThreadPoolSize_ = 0;
}

TcpInterServer::TcpInterServer(WORD Port, int ThreadPoolSize)
{
	TcpInterServer();
	Port_ = Port;
	ThreadPoolSize_ = ThreadPoolSize;

	beat_check = false;
	position = 0;
	remainBytes = HEADER_SIZE;
	isVar = false;

	poolManager = new MemPooler<msg_buffer>(10);
	if (!poolManager){
		/* error handling */
		return;
	}

	packetPoolManager = new MemPooler<CPacket>(10);
	if (!packetPoolManager){
		/* error handling */
		return;
	}
}

void TcpInterServer::Start(bool connect)
{
	Proactor_ = new Proactor;
	Acceptor_ = new Acceptor;
	Receiver_ = new Receiver;
	Sender_ = new Sender;
	Disconnector_ = new Disconnector;
	Connector_ = new Connector;

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

	Connector_->Init(Proactor_);

	isConnect = connect;
	if (connect){
		socket = new InterConnectSocket(this);
	}
	else{
		socket = new InterSocket(this);
	}
}

void TcpInterServer::Connect(char* ip, WORD port){
	if (isConnect){
		InterConnectSocket* sock = static_cast<InterConnectSocket *>(socket);
		sock->Connect(ip, port);
	}
}

void TcpInterServer::SendPlayerInfo()
{
	char* buf = poolManager->Alloc()->buf;
	int position = 0;
	char* msgBuf = poolManager->Alloc()->buf;
	int msgPosition = 0;

	/* player info send */
	ss_player_info_send msg;
	msg.type = ssType::pkt_player_info_send;

	std::list<CPlayer*>::iterator iter;
	for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
	{
		CPlayer *p = (*iter);

		player_info info;
		info.client_socket = p->Socket_;
		info.room_num = p->roomNum;
		memcpy(info.nickname, p->nickname.c_str(), sizeof(info.nickname));
		info.token = p->identifier;

		memcpy(buf + position, &info, sizeof(info));
		position += sizeof(info);
	}
	msg.player_cnt = g_vPlayers.size();
	msg.length = position + 2;

	memcpy(msgBuf + msgPosition, &msg, sizeof(msg));
	msgPosition += sizeof(msg);
	memcpy(msgBuf + msgPosition, buf, position);
	msgPosition += position;

	socket->Send(msgBuf, msgPosition);
	poolManager->Free((msg_buffer *)buf);
	poolManager->Free((msg_buffer *)msgBuf);
}

void TcpInterServer::SendRoomInfo()
{
	char* buf = poolManager->Alloc()->buf;
	int position = 0;
	char* msgBuf = poolManager->Alloc()->buf;
	int msgPosition = 0;

	/* player info send */
	ss_room_info_send msg;
	msg.type = ssType::pkt_room_info_send;

	std::list<CRoom*>::iterator iter;
	for (iter = roomManager.rooms.begin(); iter != roomManager.rooms.end(); iter++)
	{
		CRoom *p = (*iter);

		room_info info;
		info.room_num = p->roomNumber;

		memcpy(buf + position, &info, sizeof(info));
		position += sizeof(info);
	}
	msg.room_cnt = roomManager.rooms.size();
	msg.length = position + 2;

	memcpy(msgBuf + msgPosition, &msg, sizeof(msg));
	msgPosition += sizeof(msg);
	memcpy(msgBuf + msgPosition, buf, position);
	msgPosition += position;

	socket->Send(msgBuf, msgPosition);
	poolManager->Free((msg_buffer *)buf);
	poolManager->Free((msg_buffer *)msgBuf);
}

void TcpInterServer::MakeSync()
{
	SendPlayerInfo();
	SendRoomInfo();

	printf("inter server sync...\n");
}

void TcpInterServer::RecvProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		if (bytes_transferred == 0) socket->Disconnect();
		position += bytes_transferred;
		remainBytes -= bytes_transferred;

		char* buf = socket->RecvBuf_;

		if (position < HEADER_SIZE){
			socket->Recv(buf + position, remainBytes);
		}
		else{
			if (position == HEADER_SIZE){
				isVar = false;

				ssType _type = (ssType)((unsigned short)(*buf));

				switch (_type){

				case ssType::pkt_create:
					remainBytes = sizeof(ss_create)-2;
					break;
				case ssType::pkt_destroy:
					remainBytes = sizeof(ss_destroy)-2;
					break;
				case ssType::pkt_join:
					remainBytes = sizeof(ss_join)-2;
					break;
				case ssType::pkt_leave:
					remainBytes = sizeof(ss_leave)-2;
					break;
				case ssType::pkt_connect:
					remainBytes = sizeof(ss_connect)-2;
					break;
				case ssType::pkt_disconnect:
					remainBytes = sizeof(ss_disconnect)-2;
					break;
				case ssType::pkt_heartbeats:
					remainBytes = sizeof(ss_heartbeats)-2;
					break;
				case ssType::pkt_heartbeats_response:
					remainBytes = sizeof(ss_heartbeats_response)-2;
					break;
				case ssType::pkt_room_info_success:
					remainBytes = sizeof(ss_room_info_success)-2;
					break;
				case ssType::pkt_player_info_success:
					remainBytes = sizeof(ss_player_info_success)-2;
					break;
				case ssType::pkt_room_info_failure:
					remainBytes = sizeof(ss_room_info_failure)-2;
					break;
				case ssType::pkt_player_info_failure:
					remainBytes = sizeof(ss_player_info_failure)-2;
					break;

				case ssType::pkt_chat:
				case ssType::pkt_room_info_send:
				case ssType::pkt_player_info_send:
					isVar = true;
					remainBytes = sizeof(short);
					break;
				default:
					socket->Disconnect();
					break;
				}
			}
			if (remainBytes <= 0){
				if (isVar){
					unsigned short size;
					memcpy(&size, buf + 2, sizeof(short));
					remainBytes = (int)size;
					ssType _type = (ssType)((unsigned short)(*buf));
					if (_type == ssType::pkt_chat) remainBytes -= 4;
					isVar = false;
				}
				else{
					position = 0;
					remainBytes = HEADER_SIZE;

					CPacket* msg = packetPoolManager->Alloc();
					msg->owner = NULL;
					msg->msg = poolManager->Alloc()->buf;
					memcpy(msg->msg, buf, BUFSIZE);
					logicHandle.enqueue_oper(msg, true);
				}
			}
			socket->Recv(buf + position, remainBytes);
		}
	}
	else{
		/* Error Handling */
		socket->Disconnect();
	}
}

void TcpInterServer::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){

		std::list<CPlayer*>::iterator iter;
		for (iter = g_vPlayers.begin(); iter != g_vPlayers.end();)
		{
			if (!(*iter)->isMine)
			{
				if ((*iter)->roomNum != -1)
				{
					roomManager.leaveRoom((*iter), (*iter)->roomNum);
				}
				printf("delete other server's user : %d\n", (*iter)->Socket_);
				iter = g_vPlayers.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		printf("closed the other server.\n");
	}
	else{

	}
}

void TcpInterServer::SendProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){

	}
	else{

	}
}

void TcpInterServer::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){

	}
	else{

	}
}

void TcpInterServer::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){

	}
	else{

	}
}


CPlayer* find_player_by_socket(SOCKET socket)
{
	std::list<CPlayer*>::iterator iter;
	for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
	{
		CPlayer* p = *(iter);
		if (!p->isMine && p->Socket_ == socket)
		{
			return p;
		}
	}
	return NULL;
}

void TcpInterServer::packetHandling(CPacket *packet){
	ssType _type = (ssType)(*packet->msg);

	switch (_type)
	{
		case ssType::pkt_heartbeats:
		{
			printf("recieve heartbeat check. send response\n");
			char* sendMsg = poolManager->Alloc()->buf;
			ss_heartbeats_response* msg = (ss_heartbeats_response *)sendMsg;
			msg->type = ssType::pkt_heartbeats_response;
			socket->Send((char *)msg, sizeof(*msg));
			poolManager->Free((msg_buffer *)sendMsg);
			break;
		}
		case ssType::pkt_heartbeats_response:
		{
			printf("recieve heartbeat response\n");
			beat_check = true;
			break;
		}
		case ssType::pkt_connect:
		{
			printf("connect request!\n");
			ss_connect msg;
			memcpy(&msg, packet->msg, sizeof(msg));

			CPlayer* p = new CPlayer(false);
			g_vPlayers.push_back(p);
			p->Socket_ = (SOCKET)msg.client_socket;

			//printf("%d\n", g_vPlayers.size());
			break;
		}
		case ssType::pkt_disconnect:
		{
			printf("disconnect request!\n");
			ss_disconnect msg;
			memcpy(&msg, packet->msg, sizeof(msg));
			std::list<CPlayer*>::iterator iter;
			for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
			{
				if ((*iter)->Socket_ == msg.client_socket) break;
			}
			g_vPlayers.remove((*iter));

			//printf("%d\n", g_vPlayers.size());
			break;
		}
		case ssType::pkt_player_info_send:
		{
			printf("player_info_send recieve!\n");
			ss_player_info_send msg = *((ss_player_info_send *)packet->msg);

			if (msg.player_cnt + g_vPlayers.size() <= TOTAL_PLAYER)
			{
				char* buf = poolManager->Alloc()->buf;
				int position = 0;
				buf = packet->msg + sizeof(msg);

				player_info* info;
				for (int i = 0; i < msg.player_cnt; i++)
				{
					info = (player_info*)(buf + position);

					CPlayer* p = new CPlayer(false);
					g_vPlayers.push_back(p);
					p->Socket_ = (SOCKET)info->client_socket;
					p->identifier = info->token;
					p->roomNum = info->room_num;
					p->nickname = info->nickname;

					printf("player socket : %d\n", info->client_socket);
					position += sizeof(player_info);
				}

				ss_player_info_success msg;
				msg.type = ssType::pkt_player_info_success;
				socket->Send((char *)&msg, sizeof(msg));

				poolManager->Free((msg_buffer *)buf);
			}
			else{
				ss_player_info_failure msg;
				msg.type = ssType::pkt_player_info_failure;
				socket->Send((char *)&msg, sizeof(msg));
			}

			break;
		}
		case ssType::pkt_room_info_send:
		{
			printf("room_info_send recieve!\n");
			ss_room_info_send msg = *((ss_room_info_send *)packet->msg);
			if (msg.room_cnt + roomManager.rooms.size() <= ROOM_MAX)
			{
				char* buf = poolManager->Alloc()->buf;
				int position = 0;
				buf = packet->msg + sizeof(msg);

				room_info* info;
				for (int i = 0; i < msg.room_cnt; i++)
				{
					info = (room_info *)(buf + position);
					printf("create room number : %d\n", info->room_num);
					roomManager.createRoom(info->room_num);

					position += sizeof(room_info);
				}

				bool check = true;
				std::list<CPlayer*>::iterator iter;
				for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
				{
					CPlayer* p = (*iter);
					if (!p->isMine && p->roomNum != -1)
					{
						int ret = roomManager.enterRoom(p, p->roomNum);
						if (ret != -1){
							//	disconnect();
							check = false;
							break;
						}
					}
				}

				if (check)
				{
					ss_room_info_success msg;
					msg.type = ssType::pkt_room_info_success;
					socket->Send((char *)&msg, sizeof(msg));
				}
				else
				{
					ss_room_info_failure msg;
					msg.type = ssType::pkt_room_info_failure;
					socket->Send((char *)&msg, sizeof(msg));
				}
				poolManager->Free((msg_buffer *)buf);
			}
			else
			{
				ss_room_info_failure msg;
				msg.type = ssType::pkt_room_info_failure;
				socket->Send((char *)&msg, sizeof(msg));
			}

			break;
		}

		case ssType::pkt_room_info_success:
			printf("initial room sync success!\n");
			break;
		case ssType::pkt_player_info_success:
			printf("initial player info sync success!\n");
			break;

		case ssType::pkt_room_info_failure:
		case ssType::pkt_player_info_failure:
			printf("recieve fail msg!\n");
			socket->Disconnect();
			break;

		case ssType::pkt_create:
		{
			printf("create call by other server\n");
			ss_create msg = *((ss_create *)packet->msg);
			CPlayer* p = find_player_by_socket(msg.client_socket);
			if (p == NULL)
			{
				printf("not available!\n");
				break;
			}
			roomManager.createRoom(msg.room_num);
			break;
		}
		case ssType::pkt_destroy:
		{
			printf("destroy call by other server\n");
			ss_destroy msg = *((ss_destroy *)packet->msg);
			CPlayer* p = find_player_by_socket(msg.client_socket);
			if (p == NULL)
			{
				printf("not available!\n");
				break;
			}
			roomManager.destroyRoom(msg.room_num);
			break;
		}
		case ssType::pkt_join:
		{
			printf("join call by other server\n");
			ss_join msg = *((ss_join *)packet->msg);
			CPlayer* p = find_player_by_socket(msg.client_socket);
			if (p == NULL)
			{
				printf("not available!\n");
				break;
			}
			p->nickname = msg.nickname;
			roomManager.enterRoom(p, msg.room_num);
			break;
		}
		case ssType::pkt_leave:
		{
			printf("leave call by other server\n");
			ss_leave msg = *((ss_leave *)packet->msg);
			CPlayer* p = find_player_by_socket(msg.client_socket);
			if (p == NULL)
			{
				printf("not available!\n");
				break;
			}
			p->nickname = msg.nickname;
			roomManager.leaveRoom(p, msg.room_num);
			break;
		}
		case ssType::pkt_chat:
		{
			printf("chat call by other server\n");
			ss_chat msg = *((ss_chat *)packet->msg);
			pkt_type type = pkt_type::pt_chat_alarm;
			memcpy(packet->msg, &type, sizeof(short));
			roomManager.findRoom(msg.room_num)->broadcast_msg(packet->msg, msg.length);
			break;
		}
		default:
			socket->Disconnect();
			break;
	}

	this->poolManager->Free((msg_buffer *)packet->msg);
	this->packetPoolManager->Free(packet);
}