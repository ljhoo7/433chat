#pragma once

#include "stdafx.h"

extern ChatServer* chatServer;

CPlayer::CPlayer(){
	this->serverNum = chatServer->serverNum;
}

CPlayer::CPlayer(int serverNum)
{
	this->socket_ = NULL;

	this->serverNum = serverNum;
	this->nickname = "";
	this->roomNum = -1;
	this->identifier = -1;

	this->position = 0;
	this->remainBytes = HEADER_SIZE;
	this->isVar = false;

	poolManager = new MemPooler<msg_buffer>(10);
	if (!poolManager){
		PRINTF("CPlayer : MemPooler<msg_buffer> error\n");
		/* error handling */
		return;
	}

	packetPoolManager = new MemPooler<CPacket>(10);
	if (!packetPoolManager){
		PRINTF("CPlayer : MemPooler<CPacket> error\n");
		/* error handling */
		return;
	}
}

void CPlayer::RecvProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		if (bytes_transferred == 0){
			Disconnect();
		}


		this->position += bytes_transferred;
		this->remainBytes -= bytes_transferred;

		if (this->socket_ == NULL){
			PRINTF("CPlayer RecvProcess : recv buf socket is not available\n");
			/* error handling */
		}
		else{
			char* buf = this->recvBuf_;

			if (this->position < HEADER_SIZE){
				if (position == 0){
					remainBytes = HEADER_SIZE;
				}
				Recv(buf + position, remainBytes);
			}
			else{
				if (position == HEADER_SIZE){
					isVar = false;
					pkt_type _type = (pkt_type)((unsigned short)(*buf));
					switch (_type)
					{
						case pkt_type::pt_create:
							remainBytes = sizeof(t_create)-2;
							break;
						case pkt_type::pt_destroy:
							remainBytes = sizeof(t_destroy)-2;
							break;
						case pkt_type::pt_join:
							remainBytes = sizeof(t_join)-2;
							break;
						case pkt_type::pt_leave:
							remainBytes = sizeof(t_leave)-2;
							break;
						case pkt_type::pt_escape_success:
							remainBytes = sizeof(t_destroy_success)-2;
							break;
						case pkt_type::pt_escape_failure:
							remainBytes = sizeof(t_destroy_success)-2;
							break;
						case pkt_type::pt_chat:
							isVar = true;
							remainBytes = sizeof(short);
							break;
						default:
							Disconnect();
					}
				}
				
				if (remainBytes <= 0)
				{
					if (isVar)
					{
						int typePlusLength = HEADER_SIZE << 1;
						if (position == typePlusLength){
							memcpy(&remainBytes, buf + HEADER_SIZE, sizeof(short));
							remainBytes -= typePlusLength;
						}
						isVar = false;
					}
					else
					{
						position = 0;
						msg_buffer* buf_ = poolManager->Alloc();
						CPacket* msg = packetPoolManager->Alloc();
						msg->owner = this;
						msg->msg = buf_->buf;
						memcpy(msg->msg, buf, BUFSIZE);

						position = 0;
						remainBytes = HEADER_SIZE;
						chatServer->logicHandle.EnqueueOper(msg, false);
					}
				}
				Recv(buf + position, remainBytes);
			}
		}
	}
	else{
		/* error handling */
		PRINTF("CPlayer RecvProcess : Error : %d\n", WSAGetLastError());
		Disconnect();
	}
	
}

void CPlayer::SendProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		/* send complete */
	}
	else{
		PRINTF("CPlayer SendProcess : Error : %d\n", WSAGetLastError());
	}
}

void CPlayer::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		PRINTF("connect success, %d\n", this->socket_);
		this->identifier = chatServer->identifierSeed++;
		chatServer->AddUser(this);

		/* inter connection message send */
		ss_connect tmpConnect;
		tmpConnect.type = ssType::pkt_connect;
		tmpConnect.client_socket = this->socket_;
		tmpConnect.server_num = chatServer->serverNum;

		PlayerSync((char *)&tmpConnect, sizeof(ss_connect));

		if (chatServer->agentServer->socket->isConnected)
			chatServer->agentServer->socket->UserInfoSend(false, this, 1);

		Recv(this->recvBuf_, HEADER_SIZE);
	}
	else{
		/* error handling */
		PRINTF("CPlayer AcceptProcess : Error : %d\n", WSAGetLastError());
	}
}

/* 동기화 문제! */
void CPlayer::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred){
	if (!isError){
		RemovePlayer();


	}
	else{

	}
	
}

void CPlayer::RemovePlayer(){
	PRINTF("disconnect success, %d\n", this->socket_);
	CRoom* room = chatServer->roomManager.FindRoom(this->roomNum);

	if (room != NULL)
	{
		ss_leave msg;
		msg.type = ssType::pkt_leave;
		msg.client_socket = this->socket_;
		msg.room_num = this->roomNum;
		memcpy(msg.nickname, this->nickname.c_str(), sizeof(msg.nickname));

		PlayerSync((char *)&msg, sizeof(ss_leave));

		chatServer->roomManager.LeaveRoom(this, this->roomNum);
		PRINTF("remove in room..\n");
	}

	if (this->socket_ != NULL){
		ss_disconnect tmpDisconnect;

		tmpDisconnect.type = ssType::pkt_disconnect;
		tmpDisconnect.client_socket = socket_;
		tmpDisconnect.server_num = chatServer->serverNum;

		PlayerSync((char *)&tmpDisconnect, sizeof(ss_disconnect));

		if (chatServer->agentServer->socket->isConnected)
			chatServer->agentServer->socket->UserInfoSend(false, this, 0);
	}
	/* remove in list */
	chatServer->DeleteUser(this);

	this->Reuse(0);
}

void CPlayer::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){
	/* nothing to do */
}

bool CPlayer::ValidPacket(CPacket *packet)
{
	//if (this->serverNum  != chatServer->serverNum) return false;

	pkt_type _type = (pkt_type)(*packet->msg);

	t_chat tmpChat;
	t_leave tmpLeave;


	switch (_type)
	{
	case pkt_type::pt_create:
		if (this->roomNum != -1) return false;
		break;

	case pkt_type::pt_destroy:
		if (this->roomNum != -1) return false;
		break;

	case pkt_type::pt_join:
		if (this->roomNum != -1) return false;
		break;

	case pkt_type::pt_leave:
		memcpy(&tmpLeave, packet->msg, sizeof(t_leave));
		if (this->identifier != tmpLeave.token) return false;
		if (this->roomNum == -1) return false;
		if (this->roomNum != tmpLeave.room_num) return false;
		break;

	case pkt_type::pt_chat:
		memcpy(&tmpChat, packet->msg, 30);
		if (this->roomNum == -1) return false;
		if (this->identifier != tmpChat.token) return false;
		if (this->roomNum != tmpChat.room_num) return false;
	}
	return true;
}

void CPlayer::PlayerSync(char *buf, int size){
	chatServer->interServer->SendAll(buf, size);
}

void CPlayer::PacketHandling(CPacket *packet){
	if (socket_ == NULL)
	{
		return;
	}

	if (!ValidPacket(packet)){
		PRINTF("Not Valid Packet!\n");
		return;
	}

	pkt_type _type = (pkt_type)(*packet->msg);

	t_create			*tmpCreate;
	t_destroy			*tmpDestroy;
	t_join				*tmpJoin;
	t_leave				*tmpLeave;
	//	t_join_alarm		tmpJoinAlarm;
	//	t_leave_alarm		tmpLeaveAlarm;
	t_create_success	tmpCreateSuccess;
	t_create_failure	tmpCreateFailure;
	t_destroy_success	tmpDestroySuccess;
	t_destroy_failure	tmpDestroyFailure;
	t_join_success		tmpJoinSuccess;
	t_join_failure		tmpJoinFailure;
	t_leave_success		tmpLeaveSuccess;
	t_escape_success	*tmpEscapeSuccess;
	t_escape_failure	*tmpEscapeFailure;

	unsigned short		size, type;

	bool _result;

	int result;

	switch (_type)
	{
	case pkt_type::pt_escape_success:
	{
		tmpEscapeSuccess = (t_escape_success*)packet->msg;

		t_user_out tmpUserOut;
		tmpUserOut.type = pkt_type::pt_user_out_client;
		tmpUserOut.client_socket = this->socket_;
		this->Send((char*)&tmpUserOut, sizeof(t_user_out));
				
		break;
	}
	case pkt_type::pt_escape_failure:
		tmpEscapeFailure = (t_escape_failure*)packet->msg;
		break;
	case pkt_type::pt_create:
		tmpCreate = (t_create*)packet->msg;		//memcpy(&tmpCreate, packet->msg, sizeof(t_create));
		result = chatServer->roomManager.CreateRoom(tmpCreate->room_num);
		if (result == -1)
		{
			ss_create msg;
			msg.type = ssType::pkt_create;
			msg.client_socket = this->socket_;
			msg.room_num = tmpCreate->room_num;
			PlayerSync((char *)&msg, sizeof(msg));
			PRINTF("create has been sent.\n");

			//------------------------------------------------------------------------

			tmpCreateSuccess.type = pkt_type::pt_create_success;
			Send((char *)&tmpCreateSuccess, sizeof(t_create_success));
			PRINTF("create message has been sent.\n");

			if (chatServer->agentServer->socket->isConnected)
				chatServer->agentServer->socket->RoomInfoSend(false, tmpCreate->room_num, true);
			
			
		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpCreateFailure.type = pkt_type::pt_create_failure;
			tmpCreateFailure.fail_signal = fail_signal::fs_overflow;

			Send((char *)&tmpCreateFailure, sizeof(t_create_failure));
			PRINTF("create overflow message has been sent.\n");
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpCreateFailure.type = pkt_type::pt_create_failure;
			tmpCreateFailure.fail_signal = fail_signal::fs_alreadyexist;

			Send((char *)&tmpCreateFailure, sizeof(t_create_failure));
			PRINTF("create already exist message has been sent.\n");
		}
		break;
	case pkt_type::pt_destroy:
		tmpDestroy = (t_destroy*)packet->msg;		//memcpy(&tmpDestroy, packet->msg, sizeof(t_destroy));
		result = chatServer->roomManager.DestroyRoom(tmpDestroy->room_num);
		if (result == -1)
		{
			ss_destroy msg;
			msg.type = ssType::pkt_destroy;
			msg.client_socket = socket_;
			msg.room_num = tmpDestroy->room_num;
			PlayerSync((char *)&msg, sizeof(msg));

			//------------------------------------------------------------------------

			tmpDestroySuccess.type = pkt_type::pt_destroy_success;
			Send((char *)&tmpDestroySuccess, sizeof(t_destroy_success));
			PRINTF("destroy success message has been sent.\n");

			if (chatServer->agentServer->socket->isConnected)
				chatServer->agentServer->socket->RoomInfoSend(false, tmpDestroy->room_num, false);
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpDestroyFailure.type = pkt_type::pt_destroy_failure;
			tmpDestroyFailure.fail_signal = fail_signal::fs_no_exist;

			Send((char *)&tmpDestroyFailure, sizeof(t_destroy_failure));
			PRINTF("destroy no exist message has been sent.\n");
		}
		break;

	case pkt_type::pt_join:
		tmpJoin = (t_join*)packet->msg;		//memcpy(&tmpJoin, packet->msg, sizeof(t_join));
		this->nickname = tmpJoin->nickname;
		result = chatServer->roomManager.EnterRoom(this, tmpJoin->room_num);
		if (result == -1)
		{
			ss_join msg;
			msg.type = ssType::pkt_join;
			msg.client_socket = socket_;
			msg.room_num = tmpJoin->room_num;
			memcpy(msg.nickname, tmpJoin->nickname, sizeof(msg.nickname));
			PlayerSync((char *)&msg, sizeof(msg));

			PRINTF("join message has been sent.\n");

			//------------------------------------------------------------------------

			tmpJoinSuccess.type = pkt_type::pt_join_success;
			tmpJoinSuccess.token = this->identifier;

			Send((char *)&tmpJoinSuccess, sizeof(t_join_success));
			PRINTF("join success message has been sent.\n");

			if (chatServer->agentServer->socket->isConnected)
				chatServer->agentServer->socket->UserInfoSend(false, this, 2);
		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpJoinFailure.type = pkt_type::pt_join_failure;
			tmpJoinFailure.fail_signal = fail_signal::fs_overflow;

			Send((char *)&tmpJoinFailure, sizeof(t_join_failure));
			PRINTF("join overflow message has been sent.\n");
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpJoinFailure.type = pkt_type::pt_join_failure;
			tmpJoinFailure.fail_signal = fail_signal::fs_alreadyexist;

			Send((char *)&tmpJoinFailure, sizeof(t_join_failure));
			PRINTF("join alreadyexist message has been sent.\n");
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpJoinFailure.type = pkt_type::pt_join_failure;
			tmpJoinFailure.fail_signal = fail_signal::fs_no_exist;

			Send((char *)&tmpJoinFailure, sizeof(t_join_failure));
			PRINTF("join no exist message has been sent.\n");
		}
		break;

	case pkt_type::pt_leave:
		tmpLeave = (t_leave*)packet->msg;		//memcpy(&tmpLeave, packet->msg, sizeof(t_leave));
		_result = chatServer->roomManager.LeaveRoom(this, tmpLeave->room_num);

		if (_result == true)
		{
			ss_leave msg;
			msg.type = ssType::pkt_leave;
			msg.client_socket = socket_;
			msg.room_num = tmpLeave->room_num;
			msg.token = tmpLeave->token;
			memcpy(msg.nickname, tmpLeave->nickname, sizeof(msg.nickname));
			PlayerSync((char *)&msg, sizeof(msg));

			//------------------------------------------------------------------------

			tmpLeaveSuccess.type = pkt_type::pt_leave_success;
			Send((char *)&tmpLeaveSuccess, sizeof(t_leave_success));

			PRINTF("leave success message has been sent.\n");


			if (chatServer->agentServer->socket->isConnected)
				chatServer->agentServer->socket->UserInfoSend(false, this, 3);
		}
		else
		{
			PRINTF("The Leave Command has been failed. This is very extra ordinary event !\n");
		}
		break;

	case pkt_type::pt_chat:
		memcpy(&size, packet->msg + 2, sizeof(unsigned short));

		type = pkt_type::pt_chat_alarm;
		memcpy(packet->msg, &type, sizeof(unsigned short));

		chatServer->roomManager.FindRoom(this->roomNum)->BroadcastMsg(packet->msg, size);


		type = ssType::pkt_chat;
		memcpy(packet->msg, &type, sizeof(unsigned short));
		PlayerSync((char *)packet->msg, size);

		PRINTF("chat alarm message has been sent.\n");		break;
	}

	if (!poolManager->Free((msg_buffer *)packet->msg)) PRINTF("free error!\n");
	if (!packetPoolManager->Free(packet)) PRINTF("free error!\n");
}