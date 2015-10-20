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
					remainBytes = sizeof(ags_user_out);
					break;
				case sag_pkt_type::pt_room_destroy:
					remainBytes = sizeof(ags_room_destroy);
					break;
				case sag_pkt_type::pt_interserver_connect:
					remainBytes = sizeof(ags_interserver_connect);
					break;
				case sag_pkt_type::pt_interserver_disconnect:
					remainBytes = sizeof(ags_interserver_disconnect);
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
				memcpy(msg->msg, buf, position);
				chatServer->logicHandle.EnqueueOper(msg, false);

				memset(buf, 0, sizeof(buf));
				position = 0;
				remainBytes = HEADER_SIZE;
			}
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
		this->Reuse(sizeof(int));
		isConnected = false;
		/* disconn complete */
	}
	else{
		PRINTF("AgentSocket DisconnProcess : Error : %d\n", WSAGetLastError());
	}
}

void AgentSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){

}

void AgentSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		isConnected = true;
		PRINTF("agent connect success, %d\n", this->socket_);
		/* inter connection message send */		
		MakeSync();
	
		Recv(this->recvBuf_, HEADER_SIZE);
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
	
	sag_tell_agent_number msg;
	msg.agentNum = chatServer->serverNum;
	msg.type = sag_pkt_type::pt_tell_agent_number;
	Send((char *)&msg, sizeof(msg));

	UserInfoSend(true, NULL, false);
	RoomInfoSend(true, NULL, false);
	InterServerInfoSend(true, -1, false);
}

void AgentSocket::UserInfoSend(bool isTotal, CPlayer* player, bool connect){
	if (isTotal){
		sag_total_user_info msg;
		msg.type = sag_pkt_type::pt_total_user_info;
		EnterCriticalSection(&chatServer->userLock);
		msg.userCnt = chatServer->users.size();

		int size = sizeof(msg.type) + sizeof(msg.userCnt);
		int i = 0;
		std::list<CPlayer*>::iterator iter;
		for (iter = chatServer->users.begin(); iter != chatServer->users.end(); iter++)
		{
			CPlayer *p = (*iter);

			msg.userInfoList[i].roomNum = p->roomNum;
			memcpy(msg.userInfoList[i].userName, p->nickname.c_str(), sizeof(p->nickname));
			msg.userInfoList[i].userSocket = (int)p->socket_;

			i++;

			size += sizeof(SAGUserInfo);
		}

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

void AgentSocket::InterServerInfoSend(bool isTotal, int serverNum, bool connect){
	if (isTotal){
		sag_total_interserver_info msg;
		msg.type = sag_pkt_type::pt_total_interserver_info;
		msg.serverCnt = chatServer->interServer->sockets.size();

		int size = sizeof(msg.type) + sizeof(msg.serverCnt);
		int i = 0;
		for (unsigned int i = 0; i < msg.serverCnt; i++){
			InterSocket *p = chatServer->interServer->sockets[i];
			if (p->serverNum != -1){
				msg.serverNumList[i] = p->serverNum;
				i++;
				size += sizeof(msg.serverNumList[i]);
			}
		}

		Send((char *)&msg, size);
	}
	else{
		sag_interserver_connected msg;
		msg.serverNum = serverNum;
		msg.isConnected = connect;
		msg.type = sag_pkt_type::pt_interserver_connect;

		Send((char *)&msg, sizeof(msg));
	}
	
}

void AgentSocket::PacketHandling(CPacket *packet){
	if (socket_ == NULL) return;

	sag_pkt_type _type = (sag_pkt_type)(*packet->msg);

	ags_room_destroy msg;
	ags_interserver_connect msg2;
	ags_interserver_disconnect msg3;
	InterSocket* socket;

	switch (_type){
	case sag_pkt_type::pt_user_out:
		/* ���� �ؾ� �� */
		break;
	case sag_pkt_type::pt_room_destroy:
		msg = *((ags_room_destroy *)(packet->msg));
		chatServer->roomManager.DestroyRoom(msg.roomNum);
		break;
	case sag_pkt_type::pt_interserver_connect:
		msg2 = *((ags_interserver_connect *)(packet->msg));
		chatServer->interServer->Connect(msg2.serverNum);
		break;
	case sag_pkt_type::pt_interserver_disconnect:
		msg3 = *((ags_interserver_disconnect *)(packet->msg));
		socket = chatServer->interServer->GetSocketWithNum(msg3.serverNum);
		socket->Disconnect();
		break;
	default:
		this->Disconnect();
		break;
	}
}