#pragma once

#include "stdafx.h"

extern ChatServer* chatServer;

AgentSocket::AgentSocket(){
	this->serverNum = chatServer->serverNum;
}

AgentSocket::AgentSocket(int serverNum){
	this->socket_ = NULL;

	this->serverNum = serverNum;

	this->position = 0;
	this->remainBytes = HEADER_SIZE;
	this->isVar = false;

	isConnected = false;

	poolManager = new MemPooler<msg_buffer>(10);
	if (!poolManager){
		PRINTF("AgentSocket : MemPooler<msg_buffer> error\n");
		/* error handling */
		return;
	}

	packetPoolManager = new MemPooler<CPacket>(10);
	if (!packetPoolManager){
		PRINTF("AgentSocket : MemPooler<CPacket> error\n");
		/* error handling */
		return;
	}

	if (!LoadMswsock()) {
		PRINTF("Error loading mswsock functions: %d\n", WSAGetLastError());
		return;
	}
}

BOOL AgentSocket::LoadMswsock(void){
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


void AgentSocket::Connect(unsigned int ip, WORD port){
	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip; // google.com
	addr.sin_port = htons(port);

	int ok = mswsock.ConnectEx(socket_, (SOCKADDR*)&addr, sizeof(addr), &chatServer->serverNum, sizeof(serverNum), NULL,
		static_cast<OVERLAPPED*>(&act_[TcpSocket::ACT_CONNECT]));
	if (ok) {
		isConnected = true;
		PRINTF("ConnectEx succeeded immediately\n");
		ConnProcess(false, NULL, 0);
	}

	int error = WSAGetLastError();
	if (ok == FALSE && WSAGetLastError() != ERROR_IO_PENDING) {
		PRINTF("ConnectEx Error!!! s(%d), err(%d)\n", socket_, error);
	}
	PRINTF("Connect Request..\n");
}


void AgentSocket::Bind(bool reuse){
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




void AgentSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		if (bytes_transferred == 0){
			Disconnect();
		}

		position += bytes_transferred;
		remainBytes -= bytes_transferred;

		char* buf = this->recvBuf_;

		if (this->socket_ == NULL){
			PRINTF("AgentSocket RecvProcess : recv buf socket is not available\n");
			/* error handling */
			return;
		}

		if (position < HEADER_SIZE){
			this->Recv(buf + position, remainBytes);
		}
		else{
			if (position == HEADER_SIZE){
				isVar = false;
				sag_pkt_type _type = (sag_pkt_type)((unsigned short)(*buf));

				switch (_type){
				case sag_pkt_type::pt_user_out:
					PRINTF("userout!\n");
					remainBytes = sizeof(ags_user_out)-2;
					break;
				case sag_pkt_type::pt_room_destroy:
					PRINTF("room destroy!\n");
					remainBytes = sizeof(ags_room_destroy)-2;
					break;
				case sag_pkt_type::pt_kill_server:
					PRINTF("interserver connect!\n");
					remainBytes = sizeof(ags_kill_server)-2;
					break;
				case sag_pkt_type::pt_health_check:
					PRINTF("interserver disconnect!\n");
					remainBytes = sizeof(ags_health_check)-2;
					break;
				default:
					Disconnect();
					break;
				}
			}
			if (remainBytes <= 0){
				CPacket* msg = packetPoolManager->Alloc();
				msg->owner = this;
				msg->msg = poolManager->Alloc()->buf;
				msg->isAgent = true;
				memcpy(msg->msg, buf, position);
				chatServer->logicHandle.EnqueueOper(msg, false);

				memset(buf, 0, sizeof(buf));
				position = 0;
				remainBytes = HEADER_SIZE;
			}
			this->Recv(buf + position, remainBytes);
		}
	}
	else{
		/* error handling */
		PRINTF("AgentSocket RecvProcess : Error : %d\n", WSAGetLastError());
		Disconnect();
	}
}

void AgentSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){

		/* send complete */
	}
	else{
		PRINTF("AgentSocket SendProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		chatServer->agentServer->CreateConnectSocket();
		isConnected = false;
		/* disconn complete */
	}
	else{
		PRINTF("AgentSocket DisconnProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		isConnected = true;
		PRINTF("agent connect success, %d\n", this->socket_);
		/* inter connection message send */
		MakeSync();

		Recv(this->recvBuf_, HEADER_SIZE);
	}
	else{
		/* error handling */
		PRINTF("AgentSocket ConnProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		
	}
	else{
		/* error handling */
		PRINTF("AgentSocket AcceptProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::MakeSync(){
	if (socket_ == NULL){
		PRINTF("agent socket NULL!\n");
		return;
	}
	UserInfoSend(true, NULL, 0);
	RoomInfoSend(true, NULL, false);
//	InterServerInfoSend(true, -1, false);
}

void AgentSocket::UserInfoSend(bool isTotal, CPlayer* player, char connect){
	if (isTotal){
		sag_total_user_info msg;
		msg.type = sag_pkt_type::pt_total_user_info;
		EnterCriticalSection(&chatServer->userLock);
		

		int size = sizeof(msg.type) + sizeof(msg.userCnt);
		int i = 0;
		std::list<CPlayer*>::iterator iter;
		for (iter = chatServer->users.begin(); iter != chatServer->users.end(); iter++)
		{
			
			CPlayer *p = (*iter);
			if (chatServer->serverNum != p->serverNum) continue;
			msg.userInfoList[i].roomNum = p->roomNum;
			memcpy(msg.userInfoList[i].userName, p->nickname.c_str(), sizeof(p->nickname));
			msg.userInfoList[i].userSocket = (int)p->socket_;

			i++;

			size += sizeof(SAGUserInfo);
		}
		msg.userCnt = i;
		LeaveCriticalSection(&chatServer->userLock);

		Send((char*)&msg, size);
	}
	else{
		sag_user_info_changed msg;
		msg.clientSocket = (int)player->socket_;
		msg.isConnected = connect;
		msg.roomNum = player->roomNum;
		msg.type = sag_pkt_type::pt_user_info_changed;
		memcpy(msg.userName, player->nickname.c_str(), sizeof(msg.userName));

		Send((char *)&msg, sizeof(msg));
	}
	
}

void AgentSocket::RoomInfoSend(bool isTotal, int roomNum, bool create){
	if (isTotal){
		sag_total_room_info msg;
		msg.type = sag_pkt_type::pt_total_room_info;

		EnterCriticalSection(&chatServer->roomManager.roomLock);
		msg.roomCnt = chatServer->roomManager.rooms.size();

		int i = 0;
		int size = sizeof(msg.type) + sizeof(msg.roomCnt);
		std::list<CRoom*>::iterator iter;
		for (iter = chatServer->roomManager.rooms.begin(); iter != chatServer->roomManager.rooms.end(); iter++)
		{
			CRoom *p = (*iter);
			msg.roomInfoList[i].roomNum = p->roomNumber;
			i++;

			size += sizeof(SAGRoomInfo);
		}

		LeaveCriticalSection(&chatServer->roomManager.roomLock);

		Send((char*)&msg, size);
	}
	else{
		sag_room_info_changed msg;
		msg.isState = create;
		msg.roomNum = roomNum;
		msg.type = sag_pkt_type::pt_room_info_changed;

		Send((char *)&msg, sizeof(msg));
	}
	
}

//void AgentSocket::InterServerInfoSend(bool isTotal, int serverNum, bool connect){
//	if (isTotal){
//		sag_total_interserver_info msg;
//		msg.type = sag_pkt_type::pt_total_interserver_info;
//		
//
//		int size = sizeof(msg.type) + sizeof(msg.serverCnt);
//		int i = 0;
//
//		msg.serverNumList[i] = chatServer->serverNum;
//		i++;
//		size += sizeof(msg.serverNumList[i]);
//
//		for (unsigned int i = 0; i <chatServer->interServer->sockets.size(); i++){
//			InterSocket *p = chatServer->interServer->sockets[i];
//			if (p->serverNum != -1){
//				msg.serverNumList[i] = p->serverNum;
//				i++;
//				size += sizeof(msg.serverNumList[i]);
//			}
//		}
//		msg.serverCnt = i;
//		Send((char *)&msg, size);
//	}
//	else{
//		sag_server_info_changed msg;
//		msg.serverNum = serverNum;
//		msg.isConnected = connect;
//		msg.type = sag_pkt_type::pt_server_info_changed;
//
//		Send((char *)&msg, sizeof(msg));
//	}
//	
//}

void AgentSocket::PacketHandling(CPacket *packet){
	if (socket_ == NULL) return;

	sag_pkt_type _type = (sag_pkt_type)(*packet->msg);

	ags_room_destroy msg;
	ags_user_out msg4;


	switch (_type){
	case sag_pkt_type::pt_user_out:
		PRINTF("user out msg was sent!\n");

		msg4 = *((ags_user_out *)(packet->msg));
		for (std::list<CPlayer*>::iterator iter = chatServer->users.begin();
			iter != chatServer->users.end(); ++iter)
		{
			if ((*iter)->serverNum == chatServer->serverNum)
			{
				if ((*iter)->socket_ == msg4.userSocket)
				{
					t_user_out tmpUserOut;
					tmpUserOut.type = pkt_type::pt_user_out_client;
					tmpUserOut.client_socket = msg4.userSocket;
					(*iter)->Send((char*)&tmpUserOut, sizeof(t_user_out));
				}
			}
		}
		break;
	case sag_pkt_type::pt_room_destroy:
		PRINTF("room destroy msg was sent!\n");

		msg = *((ags_room_destroy *)(packet->msg));
		if (chatServer->roomManager.DestroyRoom(msg.roomNum) == -1){
			PRINTF("success delete\n");

			ss_destroy msg_;
			msg_.type = ssType::pkt_destroy;
			msg_.client_socket = socket_;
			msg_.room_num = msg.roomNum;
			chatServer->interServer->SendAll((char *)&msg_, sizeof(msg_));

			if (chatServer->agentServer->socket->isConnected)
				chatServer->agentServer->socket->RoomInfoSend(false, msg.roomNum, false);
		}
		else{
			PRINTF("fail delete\n");
		}
		break;
	case sag_pkt_type::pt_kill_server:
		PRINTF("kill_server msg was sent!\n");
		break;
	case sag_pkt_type::pt_health_check:
		PRINTF("health check msg was sent!\n");
		break;
	default:
		this->Disconnect();
		break;
	}

	if (!poolManager->Free((msg_buffer *)packet->msg)) PRINTF("free error!\n");
	if (!packetPoolManager->Free(packet)) PRINTF("free error!\n");
}