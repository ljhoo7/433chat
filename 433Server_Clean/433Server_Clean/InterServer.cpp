#include "stdafx.h"

extern CInterServer listen_server;
extern CInterServer connect_server;
extern CLogicHandle logicHandle;
extern CReceiver	receiver;

extern CRoomManager roomManager;

extern std::list<CPlayer*> g_vPlayers;

CInterServer::CInterServer() : poolManager(10), packetPoolManager(10),
process_thread(), heart_thread(), listen_thread()
{
	the_other_sock = NULL;
}

CInterServer::~CInterServer()
{
}

void CInterServer::start(int type, int port)
{
	this->type = type;

	if (type)
	{
		interserver_listen(port);
	}
	else
	{
		interserver_connect(CONNECTIP, port);
	}
}

void send_player_info(CInterServer* i)
{
	char* buf = i->poolManager.pop();
	int position = 0;
	char* msgBuf = i->poolManager.pop();
	int msgPosition = 0;

	/* player info send */
	ss_player_info_send msg;
	msg.type = ssType::pkt_player_info_send;

	std::list<CPlayer*>::iterator iter;
	for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
	{
		CPlayer *p = (*iter);

		player_info info;
		info.client_socket = p->token->clientSocket;
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

	i->_send(msgBuf, msgPosition);
	i->poolManager.push(buf);
	i->poolManager.push(msgBuf);
}

void send_room_info(CInterServer* i)
{
	char* buf = i->poolManager.pop();
	int position = 0;
	char* msgBuf = i->poolManager.pop();
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

	i->_send(msgBuf, msgPosition);
	i->poolManager.push(buf);
	i->poolManager.push(msgBuf);
}

void CInterServer::makeSync()
{
	if (the_other_sock == NULL) return;
	send_player_info(this);
	send_room_info(this);

	printf("inter server sync...\n");
}

void CInterServer::interserver_connect(char* ip, int port)
{
	printf("This server is attempting to connect...\n");
	the_other_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (the_other_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(port);

	int retval = connect(the_other_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	printf("This server has been connected to the other server by connect().\n");

	printf("InterServer Thread has been activated.\n");

	makeSync();

	if (connect_server.process_thread.joinable())
		connect_server.process_thread.join();
	process_thread = std::thread(&CInterServer::process, this);
}

void CInterServer::interserver_listen(int port)
{
	printf("This server will be waiting for The other server.\n");

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
	int retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	listen_thread = std::thread(&CInterServer::listenProcess, this);
}

void CInterServer::listenProcess()
{
	SOCKADDR_IN theotheraddr;
	int addrlen;

	while (true)
	{
		addrlen = sizeof(theotheraddr);
		the_other_sock = accept(listen_sock, (SOCKADDR*)&theotheraddr, &addrlen);
		if (the_other_sock == INVALID_SOCKET)
		{
			err_quit("accept()");
		}
		printf("This server has been connected to the other server by accept().\n");
		printf("InterServer Thread has been activated.\n");

		makeSync();

		process_thread = std::thread(&CInterServer::process, this);
		//heart_thread = std::thread(&InterServer::heartbeat_check, this);

		process_thread.join();
		//heart_thread.join();
	}

}
void CInterServer::process()
{
	int fps = 30;
	double block = 1000 / fps;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;
	long long time;

	start_time = std::chrono::system_clock::now();

	while (1)
	{
		if (the_other_sock == NULL)
		{
			printf("interserver recieve process end..\n");
			return;
		}
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time)
		{
			recieveProcess();
			start_time = std::chrono::system_clock::now();
		}
	}


}

/*
any thread
need to handle userlist lock
*/
void CInterServer::disconnect()
{
	if (the_other_sock != NULL)
	{
		std::list<CPlayer*>::iterator iter;
		for (iter = g_vPlayers.begin(); iter != g_vPlayers.end();)
		{
			if (!(*iter)->isMine)
			{
				if ((*iter)->roomNum != -1)
				{
					roomManager.leaveRoom((*iter), (*iter)->roomNum);
				}
				printf("delete other server's user : %d\n", (*iter)->token->clientSocket);
				iter = g_vPlayers.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		closesocket(the_other_sock);
		the_other_sock = NULL;

		printf("closed the other server.\n");

		//connect_server.process_thread.detach();
	}
}

/* interServer recieve thread */
void CInterServer::recieveProcess()
{
	if (the_other_sock == NULL) return;
	char* buf = poolManager.pop();
	recieve(buf, sizeof(short));
	/* packet handling */

	ssType _type = (ssType)((unsigned short)(*buf));

	switch (_type){

	case ssType::pkt_create:
		recieve(buf + 2, sizeof(ss_create) - 2);
		break;
	case ssType::pkt_destroy:
		recieve(buf + 2, sizeof(ss_destroy) - 2);
		break;
	case ssType::pkt_join:
		recieve(buf + 2, sizeof(ss_join) - 2);
		break;
	case ssType::pkt_leave:
		recieve(buf + 2, sizeof(ss_leave) - 2);
		break;
	case ssType::pkt_connect:
		recieve(buf + 2, sizeof(ss_connect) - 2);
		break;
	case ssType::pkt_disconnect:
		recieve(buf + 2, sizeof(ss_disconnect) - 2);
		break;
	case ssType::pkt_heartbeats:
		recieve(buf + 2, sizeof(ss_heartbeats) - 2);
		break;
	case ssType::pkt_heartbeats_response:
		recieve(buf + 2, sizeof(ss_heartbeats_response) - 2);
		break;
	case ssType::pkt_room_info_success:
		recieve(buf + 2, sizeof(ss_room_info_success) - 2);
		break;
	case ssType::pkt_player_info_success:
		recieve(buf + 2, sizeof(ss_player_info_success) - 2);
		break;
	case ssType::pkt_room_info_failure:
		recieve(buf + 2, sizeof(ss_room_info_failure) - 2);
		break;
	case ssType::pkt_player_info_failure:
		recieve(buf + 2, sizeof(ss_player_info_failure) - 2);
		break;

		/* var data */
	case ssType::pkt_chat:
	case ssType::pkt_room_info_send:
	case ssType::pkt_player_info_send:
		recieve(buf + 2, sizeof(short));
		unsigned short size;
		memcpy(&size, buf + 2, sizeof(short));
		recieve(buf + 4, size);
		break;
	default:
		disconnect();
		break;
	}

	CPacket* msg = packetPoolManager.pop();
	msg->type = this->type;
	msg->owner = NULL;
	msg->msg = buf;

	logicHandle.enqueue_oper(msg, true);
}
void CInterServer::recieve(char* buf, int size)
{
	if (size == 0) return;
	int ret = recv(the_other_sock, buf, size, 0);
	if (ret == SOCKET_ERROR)
	{
		printf("receive error! \n");
		disconnect();
	}
}

void CInterServer::_send(char* buf, int size)
{
	if (the_other_sock == NULL) return;
	int ret = send(the_other_sock, buf, size, 0);
	if (ret == SOCKET_ERROR)
	{
		//printf("inter-server send() error\n");
		disconnect();
	}
}

void CInterServer::heartbeat_check()
{
	while (true)
	{
		beat_check = false;
		if (the_other_sock == NULL) break;

		printf("hearth check send!\n");
		ss_heartbeats msg;
		msg.type = ssType::pkt_heartbeats;
		_send((char *)&msg, sizeof(msg));
		std::this_thread::sleep_for(std::chrono::seconds(5));
		if (beat_check == false) break;
	}
	printf("connection fail\n");
	disconnect();
}
CPlayer* find_player_by_socket(SOCKET socket)
{
	std::list<CPlayer*>::iterator iter;
	for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
	{
		CPlayer* p = *(iter);
		if (!p->isMine && p->token->clientSocket == socket)
		{
			return p;
		}
	}
	return NULL;
}

/* logic thread */
void CInterServer::packetHandling(CPacket* packet)
{
	if (the_other_sock == NULL)
	{
		return;
	}

	ssType _type = (ssType)(*packet->msg);

	switch (_type)
	{
	case ssType::pkt_heartbeats:
	{
		printf("recieve heartbeat check. send response\n");
		ss_heartbeats_response msg;
		msg.type = ssType::pkt_heartbeats_response;
		_send((char *)&msg, sizeof(msg));
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
		CUserToken *user = new CUserToken((SOCKET)msg.client_socket);
		user->peer = p;
		p->token = user;

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
			if ((*iter)->token->clientSocket == msg.client_socket) break;
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
			char* buf = poolManager.pop();
			int position = 0;
			buf = packet->msg + sizeof(msg);

			player_info* info;
			for (int i = 0; i < msg.player_cnt; i++)
			{
				info = (player_info*)(buf + position);

				CPlayer* p = new CPlayer(false);
				g_vPlayers.push_back(p);
				CUserToken *user = new CUserToken((SOCKET)info->client_socket);
				user->peer = p;
				p->token = user;
				p->identifier = info->token;
				p->roomNum = info->room_num;
				p->nickname = info->nickname;

				printf("player socket : %d\n", info->client_socket);
				position += sizeof(player_info);
			}

			ss_player_info_success msg;
			msg.type = ssType::pkt_player_info_success;
			this->_send((char *)&msg, sizeof(msg));
			poolManager.push(buf);
		}
		else{
			ss_player_info_failure msg;
			msg.type = ssType::pkt_player_info_failure;
			this->_send((char *)&msg, sizeof(msg));
		}

		break;
	}
	case ssType::pkt_room_info_send:
	{
		printf("room_info_send recieve!\n");
		ss_room_info_send msg = *((ss_room_info_send *)packet->msg);
		if (msg.room_cnt + roomManager.rooms.size() <= ROOM_MAX)
		{
			char* buf = poolManager.pop();
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
				this->_send((char *)&msg, sizeof(msg));
				poolManager.push(buf);
			}
			else
			{
				ss_room_info_failure msg;
				msg.type = ssType::pkt_room_info_failure;
				this->_send((char *)&msg, sizeof(msg));
			}
		}
		else
		{
			ss_room_info_failure msg;
			msg.type = ssType::pkt_room_info_failure;
			this->_send((char *)&msg, sizeof(msg));
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
		disconnect();
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
		disconnect();
		break;
	}

	this->packetPoolManager.push(packet);
}