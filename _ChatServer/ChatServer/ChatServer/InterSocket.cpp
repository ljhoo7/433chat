#include "stdafx.h"

BOOL InterSocket::LoadMswsock(void){
	SOCKET sock;
	DWORD dwBytes;
	int rc;

	/* Dummy socket needed for WSAIoctl */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return FALSE;

	{
		GUID guid = WSAID_CONNECTEX;
		rc = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guid, sizeof(guid),
			&mswsock.ConnectEx, sizeof(mswsock.ConnectEx),
			&dwBytes, NULL, NULL);
		if (rc != 0)
			return FALSE;
	}

	rc = closesocket(sock);
	if (rc != 0)
		return FALSE;

	return TRUE;
}

InterSocket::InterSocket(TcpInterServer* InterServer, bool isConnect){
	this->isConnect = isConnect;
	interServer_ = InterServer;
	serverNum = -1;

	position = 0;
	remainBytes = HEADER_SIZE;
	isVar = false;

	poolManager = new MemPooler<msg_buffer>(1000);
	if (!poolManager){
		/* error handling */
		return;
	}

	packetPoolManager = new MemPooler<CPacket>(1000);
	if (!packetPoolManager){
		/* error handling */
		return;
	}

	if (isConnect){
		if (!LoadMswsock()) {
			PRINTF("Error loading mswsock functions: %d\n", WSAGetLastError());
			return;
		}
	}
}


void InterSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		if (bytes_transferred == 0){
			this->Disconnect();
			return;
		}

		//	PRINTF("interServer recieve complete\n");

		position += bytes_transferred;
		remainBytes -= bytes_transferred;

		char* buf = this->recvBuf_;

		if (position < HEADER_SIZE){
			this->Recv(buf + position, remainBytes);
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
					this->Disconnect();
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
					msg->owner = this;
					msg->msg = poolManager->Alloc()->buf;
					memcpy(msg->msg, buf, BUFSIZE);
					chatServer->logicHandle.EnqueueOper(msg, true);
				}
			}
			this->Recv(buf + position, remainBytes);
		}
	}
	else{
		PRINTF("interServer recieve error\n");
		/* Error Handling */
		this->Disconnect();
	}
}

void InterSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		//PRINTF("interServer send complete\n");
	}
	else{
		PRINTF("interServer send error\n");
	}
}

void InterSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		isUse = true;
		serverNum = chatServer->GetServerNum((unsigned int)addr_.sin_addr.s_addr, addr_.sin_port);
		
		PRINTF("connect with %d server\n", serverNum);
		MakeSync();
		Recv(recvBuf_, HEADER_SIZE);

		//	heartThread = std::thread(&TcpInterServer::heartbeatCheck, this);
	}
	else{
		PRINTF("interServer accept error\n");
	}
}

void InterSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		isUse = false;
		serverNum = -1;

		EnterCriticalSection(&chatServer->userLock);
		std::list<CPlayer*>::iterator iter;
		for (iter = chatServer->users.begin(); iter != chatServer->users.end();)
		{
			if (!(*iter)->isMine)
			{
				if ((*iter)->roomNum != -1)
				{
					chatServer->roomManager.LeaveRoom((*iter), (*iter)->roomNum);
				}
				PRINTF("delete other server's user : %d\n", (*iter)->socket_);
				iter = chatServer->users.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		LeaveCriticalSection(&chatServer->userLock);

		PRINTF("closed the other server.\n");

		//if (heartThread.joinable()) heartThread.join();
		if (!isConnect) this->Reuse();
		else{
			this->Bind(false);
			proactor_->Register((HANDLE)this->socket_);
		}
	}
	else{
		PRINTF("interServer disconnect error\n");
	}
}

void InterSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		isUse = true;

		PRINTF("connect with %d server\n", serverNum);

		MakeSync();
		Recv(recvBuf_, HEADER_SIZE);

		//	heartThread = std::thread(&TcpInterServer::heartbeatCheck, this);
	}
	else{
		serverNum = -1;
		PRINTF("interServer connect error\n");
	}
}

void  InterSocket::Bind(bool reuse){
	if (!isConnect){
		PRINTF("only for connection socket\n");
		return;
	}

	if (!reuse){
		socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (socket_ == INVALID_SOCKET)
		{
			PRINTF("WSASocket() Error!!! err(%d)\n", WSAGetLastError());
		}

	}
	int rc;
	struct sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;
	rc = bind(socket_, (SOCKADDR*)&addr, sizeof(addr));
	if (rc != 0) {
		PRINTF("bind failed: %d\n", WSAGetLastError());
		return;
	}
}


void InterSocket::Connect(unsigned int ip, WORD port){
	if (!isConnect){
		PRINTF("only for connection socket\n");
		return;
	}

	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip; // google.com
	addr.sin_port = htons(port);

	int ok = mswsock.ConnectEx(socket_, (SOCKADDR*)&addr, sizeof(addr), NULL, 0, NULL,
		static_cast<OVERLAPPED*>(&act_[TcpSocket::ACT_CONNECT]));
	if (ok) {
		PRINTF("ConnectEx succeeded immediately\n");
	}

	int error = WSAGetLastError();
	if (ok == FALSE && WSAGetLastError() != ERROR_IO_PENDING) {
		PRINTF("ConnectEx Error!!! s(%d), err(%d)\n", socket_, error);
	}
	PRINTF("Connect Request..\n");
}

void InterSocket::MakeSync()
{
	SendPlayerInfo();
	SendRoomInfo();

	PRINTF("inter server sync...\n");
}

void InterSocket::SendPlayerInfo()
{
	char* buf = poolManager->Alloc()->buf;
	int position = 0;
	char* msgBuf = poolManager->Alloc()->buf;
	int msgPosition = 0;

	/* player info send */
	ss_player_info_send msg;
	msg.type = ssType::pkt_player_info_send;

	EnterCriticalSection(&chatServer->userLock);

	std::list<CPlayer*>::iterator iter;
	for (iter = chatServer->users.begin(); iter != chatServer->users.end(); iter++)
	{
		CPlayer *p = (*iter);

		player_info info;
		info.client_socket = p->socket_;
		info.room_num = p->roomNum;
		memcpy(info.nickname, p->nickname.c_str(), sizeof(info.nickname));
		info.token = p->identifier;

		memcpy(buf + position, &info, sizeof(info));
		position += sizeof(info);
	}
	msg.player_cnt = chatServer->users.size();
	msg.length = position + 2;

	memcpy(msgBuf + msgPosition, &msg, sizeof(msg));
	msgPosition += sizeof(msg);
	memcpy(msgBuf + msgPosition, buf, position);
	msgPosition += position;

	LeaveCriticalSection(&chatServer->userLock);

	this->Send(msgBuf, msgPosition);

	if (!poolManager->Free((msg_buffer *)buf)) PRINTF("free error!\n");
	if (!poolManager->Free((msg_buffer *)msgBuf)) PRINTF("free error!\n");
}

void InterSocket::SendRoomInfo()
{
	char* buf = poolManager->Alloc()->buf;
	int position = 0;
	char* msgBuf = poolManager->Alloc()->buf;
	int msgPosition = 0;

	/* player info send */
	ss_room_info_send msg;
	msg.type = ssType::pkt_room_info_send;

	std::list<CRoom*>::iterator iter;
	for (iter = chatServer->roomManager.rooms.begin(); iter != chatServer->roomManager.rooms.end(); iter++)
	{
		CRoom *p = (*iter);

		room_info info;
		info.room_num = p->roomNumber;

		memcpy(buf + position, &info, sizeof(info));
		position += sizeof(info);
	}
	msg.room_cnt = chatServer->roomManager.rooms.size();
	msg.length = position + 2;

	memcpy(msgBuf + msgPosition, &msg, sizeof(msg));
	msgPosition += sizeof(msg);
	memcpy(msgBuf + msgPosition, buf, position);
	msgPosition += position;

	this->Send(msgBuf, msgPosition);
	if (!poolManager->Free((msg_buffer *)buf)) PRINTF("free error!\n");
	if (!poolManager->Free((msg_buffer *)msgBuf)) PRINTF("free error!\n");
}


CPlayer* find_player_by_socket(SOCKET socket)
{
	return chatServer->FindUser(socket);
}

void InterSocket::packetHandling(CPacket *packet){
	if (socket_ == NULL) return;

	ssType _type = (ssType)(*packet->msg);

	switch (_type)
	{
		case ssType::pkt_heartbeats:
		{
			PRINTF("recieve heartbeat check. send response\n");
			char* sendMsg = poolManager->Alloc()->buf;
			ss_heartbeats_response* msg = (ss_heartbeats_response *)sendMsg;
			msg->type = ssType::pkt_heartbeats_response;
			Send((char *)msg, sizeof(*msg));
			if (!poolManager->Free((msg_buffer *)sendMsg)) PRINTF("free error!\n");
			break;
		}
	case ssType::pkt_heartbeats_response:
	{
		PRINTF("recieve heartbeat response\n");
		//beatCheck = true;
		break;
	}
	case ssType::pkt_connect:
	{
		PRINTF("connect request!\n");
		ss_connect msg;
		memcpy(&msg, packet->msg, sizeof(msg));

		CPlayer* p = new CPlayer(false);
		chatServer->AddUser(p);
		p->socket_ = (SOCKET)msg.client_socket;

		//PRINTF("%d\n", g_vPlayers.size());
		break;
	}
	case ssType::pkt_disconnect:
	{
		PRINTF("disconnect request!\n");
		ss_disconnect msg;
		memcpy(&msg, packet->msg, sizeof(msg));

		chatServer->DeleteUser(chatServer->FindUser(msg.client_socket));

		//PRINTF("%d\n", g_vPlayers.size());
		break;
	}
	case ssType::pkt_player_info_send:
	{
		PRINTF("player_info_send recieve!\n");
		ss_player_info_send msg = *((ss_player_info_send *)packet->msg);

		if (msg.player_cnt + chatServer->GetUserCnt() <= TOTAL_PLAYER)
		{
			char* buf = poolManager->Alloc()->buf;
			int position = 0;
			buf = packet->msg + sizeof(msg);

			player_info* info;
			for (int i = 0; i < msg.player_cnt; i++)
			{
				info = (player_info*)(buf + position);

				CPlayer* p = new CPlayer(false);
				chatServer->AddUser(p);
				p->socket_ = (SOCKET)info->client_socket;
				p->identifier = info->token;
				p->roomNum = info->room_num;
				p->nickname = info->nickname;

				PRINTF("player socket : %d\n", info->client_socket);
				position += sizeof(player_info);
			}

			ss_player_info_success msg;
			msg.type = ssType::pkt_player_info_success;
			this->Send((char *)&msg, sizeof(msg));

			if (!poolManager->Free((msg_buffer *)buf)) PRINTF("free error!\n");;
		}
		else{
			ss_player_info_failure msg;
			msg.type = ssType::pkt_player_info_failure;
			this->Send((char *)&msg, sizeof(msg));
		}

		break;
	}
	case ssType::pkt_room_info_send:
	{
		PRINTF("room_info_send recieve!\n");
		ss_room_info_send msg = *((ss_room_info_send *)packet->msg);
		if (msg.room_cnt + chatServer->roomManager.rooms.size() <= ROOM_MAX)
		{
			char* buf = poolManager->Alloc()->buf;
			int position = 0;
			buf = packet->msg + sizeof(msg);

			room_info* info;
			for (int i = 0; i < msg.room_cnt; i++)
			{
				info = (room_info *)(buf + position);
				PRINTF("create room number : %d\n", info->room_num);
				chatServer->roomManager.CreateRoom(info->room_num);

				position += sizeof(room_info);
			}

			bool check = chatServer->DeleteOtherServerUsers(-1);


			if (check)
			{
				ss_room_info_success msg;
				msg.type = ssType::pkt_room_info_success;
				this->Send((char *)&msg, sizeof(msg));
			}
			else
			{
				ss_room_info_failure msg;
				msg.type = ssType::pkt_room_info_failure;
				this->Send((char *)&msg, sizeof(msg));
			}
			if (!poolManager->Free((msg_buffer *)buf)) PRINTF("free error!\n");;
		}
		else
		{
			ss_room_info_failure msg;
			msg.type = ssType::pkt_room_info_failure;
			this->Send((char *)&msg, sizeof(msg));
		}

		break;
	}

	case ssType::pkt_room_info_success:
		PRINTF("initial room sync success!\n");
		break;
	case ssType::pkt_player_info_success:
		PRINTF("initial player info sync success!\n");
		break;

	case ssType::pkt_room_info_failure:
	case ssType::pkt_player_info_failure:
		PRINTF("recieve fail msg!\n");
		this->Disconnect();
		break;

	case ssType::pkt_create:
	{
		PRINTF("create call by other server\n");
		ss_create msg = *((ss_create *)packet->msg);
		CPlayer* p = find_player_by_socket(msg.client_socket);
		if (p == NULL)
		{
			PRINTF("not available!\n");
			break;
		}
		chatServer->roomManager.CreateRoom(msg.room_num);
		break;
	}
	case ssType::pkt_destroy:
	{
		PRINTF("destroy call by other server\n");
		ss_destroy msg = *((ss_destroy *)packet->msg);
		CPlayer* p = find_player_by_socket(msg.client_socket);
		if (p == NULL)
		{
			PRINTF("not available!\n");
			break;
		}
		chatServer->roomManager.DestroyRoom(msg.room_num);
		break;
	}
	case ssType::pkt_join:
	{
		PRINTF("join call by other server\n");
		ss_join msg = *((ss_join *)packet->msg);
		CPlayer* p = find_player_by_socket(msg.client_socket);
		if (p == NULL)
		{
			PRINTF("not available!\n");
			break;
		}
		p->nickname = msg.nickname;
		chatServer->roomManager.EnterRoom(p, msg.room_num);
		break;
	}
	case ssType::pkt_leave:
	{
		PRINTF("leave call by other server\n");
		ss_leave msg = *((ss_leave *)packet->msg);
		CPlayer* p = find_player_by_socket(msg.client_socket);
		if (p == NULL)
		{
			PRINTF("not available!\n");
			break;
		}
		p->nickname = msg.nickname;
		chatServer->roomManager.LeaveRoom(p, msg.room_num);
		break;
	}
	case ssType::pkt_chat:
	{
		PRINTF("chat call by other server\n");
		ss_chat msg = *((ss_chat *)packet->msg);
		pkt_type type = pkt_type::pt_chat_alarm;
		memcpy(packet->msg, &type, sizeof(short));
		chatServer->roomManager.FindRoom(msg.room_num)->BroadcastMsg(packet->msg, msg.length);
		break;
	}
	default:
		this->Disconnect();
		break;
	}

	if (!this->poolManager->Free((msg_buffer *)packet->msg)) PRINTF("free error!\n");;
	if (!this->packetPoolManager->Free(packet)) PRINTF("free error!\n");;
}
