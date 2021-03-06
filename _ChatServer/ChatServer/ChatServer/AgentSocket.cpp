#pragma once

#include "stdafx.h"

extern ChatServer* chatServer;

AgentSocket::AgentSocket()
{
	this->serverNum = chatServer->serverNum;
}

AgentSocket::AgentSocket(int serverNum)
{
	this->socket_ = NULL;

	this->serverNum = serverNum;

	this->position = 0;
	this->remainBytes = HEADER_SIZE;
	this->isVar = false;

	isConnected = false;

	poolManager = new MemPooler<msg_buffer>(10);
	if (!poolManager)
	{
		PRINT("[AgentSocket] MemPooler<msg_buffer> error\n");
		/* error handling */
		return;
	}

	packetPoolManager = new MemPooler<CPacket>(10);
	if (!packetPoolManager)
	{
		PRINT("[AgentSocket] MemPooler<CPacket> error\n");
		/* error handling */
		return;
	}

	if (!LoadMswsock()) 
	{
		PRINT("[AgentSocket] Error loading mswsock functions: %d\n", WSAGetLastError());
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
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	int ok = mswsock.ConnectEx(socket_, (SOCKADDR*)&addr, sizeof(addr), &chatServer->serverNum, sizeof(serverNum), NULL,
		static_cast<OVERLAPPED*>(&act_[TcpSocket::ACT_CONNECT]));
	if (ok) 
	{
		isConnected = true;
		PRINT("[AgentSocket] ConnectEx succeeded immediately\n");
		ConnProcess(false, NULL, 0);
	}

	int error = WSAGetLastError();
	if (ok == FALSE && WSAGetLastError() != ERROR_IO_PENDING) 
	{
		PRINT("[AgentSocket] ConnectEx Error!!! s(%d), err(%d)\n", socket_, error);
	}
	PRINT("[AgentSocket] Connect Request..\n");
}


void AgentSocket::Bind(bool reuse)
{
	if (!reuse)
	{
		socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (socket_ == INVALID_SOCKET)
		{
			PRINT("[AgentSocket] WSASocket() Error!!! err(%d)\n", WSAGetLastError());
		}

	}
	int rc;
	struct sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;
	rc = bind(socket_, (SOCKADDR*)&addr, sizeof(addr));

	if (rc != 0) 
	{
		PRINT("[AgentSocket] bind failed: %d\n", WSAGetLastError());
		return;
	}
}




void AgentSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError)
	{
		if (bytes_transferred == 0)
		{
			Disconnect();
		}

		position += bytes_transferred;
		remainBytes -= bytes_transferred;

		char* buf = this->recvBuf_;

		if (this->socket_ == NULL)
		{
			PRINT("[AgentSocket] RecvProcess : recv buf socket is not available\n");
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
					remainBytes = sizeof(ags_user_out)-2;
					break;
				case sag_pkt_type::pt_room_destroy:
					remainBytes = sizeof(ags_room_destroy)-2;
					break;
				case sag_pkt_type::pt_kill_server:
					remainBytes = sizeof(ags_kill_server)-2;
					break;
				case sag_pkt_type::pt_health_check:
					remainBytes = sizeof(ags_health_check)-2;
					break;
				default:
					//PRINT("[AgentSocket] disconnect 5\n");
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
		PRINT("[AgentSocket] RecvProcess : Error : %d\n", WSAGetLastError());

		Disconnect();
	}
}

void AgentSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){

		/* send complete */
	}
	else{
		PRINT("[AgentSocket] SendProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		PRINT("[AgentSocket] agent disconnect success\n");

		chatServer->agentServer->CreateConnectSocket();
		isConnected = false;
		/* disconn complete */

		if (chatServer->isEnd){
			chatServer->EndServer();
		}
	}
	else{
		PRINT("[AgentSocket] DisconnProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		isConnected = true;
		PRINT("[AgentSocket] agent connect success, %d\n", this->socket_);
		/* inter connection message send */
		MakeSync();

		Recv(this->recvBuf_, HEADER_SIZE);
	}
	else{
		/* error handling */
		PRINT("[AgentSocket] ConnProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		
	}
	else{
		/* error handling */
		PRINT("[AgentSocket] AcceptProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::MakeSync(){
	if (socket_ == NULL){
		PRINT("[AgentSocket] agent socket NULL!\n");
		return;
	}
	UserInfoSend(true, NULL, 0);
	RoomInfoSend(true, NULL, false);
//	InterServerInfoSend(true, -1, false);
}

void AgentSocket::UserInfoSend(bool isTotal, CPlayer* player, char connect){
	if (isTotal){
		sag_total_user_info msg;
		int size;
		
		chatServer->clientServer->TotalUserInfo(&msg, &size);
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
		int size;
		chatServer->roomManager.TotalRoomInfo(&msg, &size);

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
	


	switch (_type){
	case sag_pkt_type::pt_user_out:
		PRINT("[AgentSocket] user out msg was sent!\n");
		ags_user_out msg4;
		msg4 = *((ags_user_out *)(packet->msg));
		
		chatServer->clientServer->UserOut(msg4.userSocket);
		break;
	case sag_pkt_type::pt_room_destroy:
		PRINT("[AgentSocket] room destroy msg was sent!\n");

		msg = *((ags_room_destroy *)(packet->msg));
		if (chatServer->roomManager.DestroyRoom(msg.roomNum) == -1){
			PRINT("[AgentSocket] success delete\n");

			ss_destroy msg_;
			msg_.type = ssType::pkt_destroy;
			msg_.client_socket = socket_;
			msg_.room_num = msg.roomNum;
			chatServer->interServer->SendAll((char *)&msg_, sizeof(msg_));
		}
		else{
			PRINT("[AgentSocket] fail delete\n");
		}
		break;
	case sag_pkt_type::pt_kill_server:
		PRINT("[AgentSocket] kill_server msg was sent!\n");
		chatServer->isEnd = true;
		chatServer->EndServer();
		break;
	case sag_pkt_type::pt_health_check:
#ifdef HEARTBEAT
		PRINT("[AgentSocket] health check msg was sent!\n");
#endif
		sag_health_ack msg;
		msg.type = sag_pkt_type::pt_health_ack;

		Send((char *)&msg, sizeof(msg));
		break;
	default:
		//PRINT("[AgentSocket] disconnect 6\n");
		this->Disconnect();
		break;
	}

	if (!poolManager->Free((msg_buffer *)packet->msg)) PRINT("[AgentSocket] free error!\n");
	if (!packetPoolManager->Free(packet)) PRINT("[AgentSocket] free error!\n");
}