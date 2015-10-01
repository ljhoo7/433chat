#include "stdafx.h"

extern CRoomManager roomManager;
extern CLogicHandle logicHandle;

extern CInterServer listen_server;
extern CInterServer connect_server;

extern CReceiver receiver;

extern int identifier_seed;
extern std::list<CPlayer*> g_vPlayers;

CPlayer::CPlayer(bool isMine) : poolManager(10), packetPoolManager(10)
{
	this->isMine = isMine;
	this->nickname = "";
	this->roomNum = -1;
	this->identifier = identifier_seed++;
}

void CPlayer::recieveProcess()
{
	PPerIoContext t = &this->token->SocketContext.recvContext;
	if (token->clientSocket == NULL)
	{
		std::cout << "There is a weired UserToken which has note connecd socket." << std::endl;
		return;
	}
	char* buf = this->token->SocketContext.recvContext.Buffer;

	if (t->position < HEADER_SIZE)
	{
		if (t->position == 0){
			t->remainBytes = HEADER_SIZE;
		}
		this->token->_recieve(buf + t->position, t->remainBytes);
		return;
	}
	else
	{
		if (t->position == HEADER_SIZE){
			t->isVar = FALSE;
			pkt_type _type = (pkt_type)((unsigned short)(*buf));
			switch (_type)
			{
			case pkt_type::pt_create:
				t->remainBytes = sizeof(t_create)-2;
				break;
			case pkt_type::pt_destroy:
				t->remainBytes = sizeof(t_destroy)-2;
				break;
			case pkt_type::pt_join:
				t->remainBytes = sizeof(t_join)-2;
				break;
			case pkt_type::pt_leave:
				t->remainBytes = sizeof(t_leave)-2;
				break;
			case pkt_type::pt_chat:
				t->isVar = TRUE;
				t->remainBytes = sizeof(short);
				break;
			default:
				//disconnect();
				this->remove();
				return;
			}
		}
		if (t->remainBytes <= 0)
		{
			if (t->isVar)
			{
				int typePlusLength = HEADER_SIZE << 1;
				if (t->position == typePlusLength){
					memcpy(&t->remainBytes, buf + HEADER_SIZE, sizeof(short));
					t->remainBytes -= typePlusLength;
				}
				t->isVar = false;
			}
			else
			{
				char* buf = poolManager.pop();
				CPacket* msg = packetPoolManager.pop();
				msg->type = 2;
				msg->owner = this->token;
				msg->msg = buf;
				memcpy(buf, t->Buffer, BUFSIZE);
				logicHandle.enqueue_oper(msg, false);

				t->position = 0;
				t->remainBytes = HEADER_SIZE;
			}
		}
		this->token->_recieve(buf + t->position, t->remainBytes);
		return;
	}
	return;
}

void CPlayer::disconnect()
{
	if (token->clientSocket != NULL)
	{
		printf("closed a connection with a client.\n");
		closesocket(token->clientSocket);
		token->clientSocket = NULL;
	}
}

void CPlayer::send_msg(char *buf, int size)
{
	WSABUF temp;
	temp.buf = buf;
	temp.len = size;

	token->SocketContext.sendContext.wsaBuf = &temp;

	DWORD dwSendBytes = 0;
	DWORD dwFlags = 0;

	ZeroMemory(&token->SocketContext.sendContext.overlapped, sizeof(WSAOVERLAPPED));

	int ret = WSASend(token->clientSocket, token->SocketContext.sendContext.wsaBuf, 1,
		&dwSendBytes, dwFlags, &(token->SocketContext.sendContext.overlapped), NULL);

	//WSABUFPoolManager->Free(wsabuf);

	if (SOCKET_ERROR == ret)
	{
		int ErrCode = WSAGetLastError();
		if (ErrCode != WSA_IO_PENDING)
		{
			err_quit("interserver send queue process error!");
			return;
		}
	}
}

void CPlayer::recieve_msg(char* buf, int size)
{
}

bool CPlayer::valid_Packet(CPacket *packet)
{
	if (!this->isMine) return false;

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

void CPlayer::playerSync(char *buf, int size)
{
	if (listen_server.the_other_sock != NULL)
	{
		listen_server._send(buf, size);
	}
	if (connect_server.the_other_sock != NULL)
	{
		connect_server._send(buf, size);
	}
}

void CPlayer::packetHandling(CPacket *packet)
{
	if (token->clientSocket == NULL)
	{
		return;
	}

	if (!valid_Packet(packet)) return;

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

	unsigned short		size, type;

	bool _result;

	int result;

	switch (_type)
	{
	case pkt_type::pt_create:
		tmpCreate = (t_create*)packet->msg;		//memcpy(&tmpCreate, packet->msg, sizeof(t_create));
		result = roomManager.createRoom(tmpCreate->room_num);
		if (result == -1)
		{
			ss_create* msg = (ss_create *)poolManager.pop();
			msg->type = ssType::pkt_create;
			msg->client_socket = token->clientSocket;
			msg->room_num = tmpCreate->room_num;
			playerSync((char *)msg, sizeof(*msg));
			std::cout << "create has been sent." << std::endl;

			//------------------------------------------------------------------------

			tmpCreateSuccess.type = pkt_type::pt_create_success;
			send_msg((char *)&tmpCreateSuccess, sizeof(t_create_success));
			std::cout << "create message has been sent." << std::endl;
		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpCreateFailure.type = pkt_type::pt_create_failure;
			tmpCreateFailure.fail_signal = fail_signal::fs_overflow;

			send_msg((char *)&tmpCreateFailure, sizeof(t_create_failure));
			std::cout << "create overflow message has been sent." << std::endl;
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpCreateFailure.type = pkt_type::pt_create_failure;
			tmpCreateFailure.fail_signal = fail_signal::fs_alreadyexist;

			send_msg((char *)&tmpCreateFailure, sizeof(t_create_failure));
			std::cout << "create already exist message has been sent." << std::endl;
		}
		break;
	case pkt_type::pt_destroy:
		tmpDestroy = (t_destroy*)packet->msg;		//memcpy(&tmpDestroy, packet->msg, sizeof(t_destroy));
		result = roomManager.destroyRoom(tmpDestroy->room_num);
		if (result == -1)
		{
			ss_destroy* msg = (ss_destroy*)poolManager.pop();
			msg->type = ssType::pkt_destroy;
			msg->client_socket = token->clientSocket;
			msg->room_num = tmpDestroy->room_num;
			playerSync((char *)msg, sizeof(*msg));

			//------------------------------------------------------------------------

			tmpDestroySuccess.type = pkt_type::pt_destroy_success;
			send_msg((char *)&tmpDestroySuccess, sizeof(t_destroy_success));
			std::cout << "destroy success message has been sent." << std::endl;
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpDestroyFailure.type = pkt_type::pt_destroy_failure;
			tmpDestroyFailure.fail_signal = fail_signal::fs_no_exist;

			send_msg((char *)&tmpDestroyFailure, sizeof(t_destroy_failure));
			std::cout << "destroy no exist message has been sent." << std::endl;
		}
		break;

	case pkt_type::pt_join:
		tmpJoin = (t_join*)packet->msg;		//memcpy(&tmpJoin, packet->msg, sizeof(t_join));
		this->nickname = tmpJoin->nickname;
		result = roomManager.enterRoom(this, tmpJoin->room_num);
		if (result == -1)
		{
			ss_join* msg = (ss_join *)poolManager.pop();
			msg->type = ssType::pkt_join;
			msg->client_socket = token->clientSocket;
			msg->room_num = tmpJoin->room_num;
			memcpy(msg->nickname, tmpJoin->nickname, sizeof(msg->nickname));
			playerSync((char *)msg, sizeof(*msg));

			std::cout << "join message has been sent." << std::endl;

			//------------------------------------------------------------------------

			tmpJoinSuccess.type = pkt_type::pt_join_success;
			tmpJoinSuccess.token = this->identifier;

			send_msg((char *)&tmpJoinSuccess, sizeof(t_join_success));
			std::cout << "join success message has been sent." << std::endl;


		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpJoinFailure.type = pkt_type::pt_join_failure;
			tmpJoinFailure.fail_signal = fail_signal::fs_overflow;

			send_msg((char *)&tmpJoinFailure, sizeof(t_join_failure));
			std::cout << "join overflow message has been sent." << std::endl;
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpJoinFailure.type = pkt_type::pt_join_failure;
			tmpJoinFailure.fail_signal = fail_signal::fs_alreadyexist;

			send_msg((char *)&tmpJoinFailure, sizeof(t_join_failure));
			std::cout << "join alreadyexist message has been sent." << std::endl;
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpJoinFailure.type = pkt_type::pt_join_failure;
			tmpJoinFailure.fail_signal = fail_signal::fs_no_exist;

			send_msg((char *)&tmpJoinFailure, sizeof(t_join_failure));
			std::cout << "join no exist message has been sent." << std::endl;
		}
		break;

	case pkt_type::pt_leave:
		tmpLeave = (t_leave*)packet->msg;		//memcpy(&tmpLeave, packet->msg, sizeof(t_leave));
		_result = roomManager.leaveRoom(this, tmpLeave->room_num);

		if (_result == true)
		{
			ss_leave* msg = (ss_leave *)poolManager.pop();
			msg->type = ssType::pkt_leave;
			msg->client_socket = token->clientSocket;
			msg->room_num = tmpLeave->room_num;
			msg->token = tmpLeave->token;
			memcpy(msg->nickname, tmpLeave->nickname, sizeof(msg->nickname));
			playerSync((char *)msg, sizeof(*msg));

			//------------------------------------------------------------------------

			tmpLeaveSuccess.type = pkt_type::pt_leave_success;
			send_msg((char *)&tmpLeaveSuccess, sizeof(t_leave_success));

			std::cout << "leave success message has been sent." << std::endl;
		}
		else
		{
			std::cout << "The Leave Command has been failed. This is very extra ordinary event !" << std::endl;
		}
		break;

	case pkt_type::pt_chat:
		memcpy(&size, packet->msg + 2, sizeof(unsigned short));

		type = pkt_type::pt_chat_alarm;
		memcpy(packet->msg, &type, sizeof(unsigned short));

		roomManager.findRoom(this->roomNum)->broadcast_msg(packet->msg, size);


		type = ssType::pkt_chat;
		memcpy(packet->msg, &type, sizeof(unsigned short));
		playerSync((char *)packet->msg, size);

		std::cout << "chat alarm message has been sent." << std::endl;		break;
	}

	//this->poolManager.push(packet->msg);
	this->packetPoolManager.push(packet);
}

void CPlayer::remove()
{
	CRoom* room = roomManager.findRoom(this->roomNum);

	if (room != NULL)
	{
		//-----------------------------------------------
		char* sendMsg = poolManager.pop();
		ss_leave* msg = (ss_leave*)sendMsg;
		msg->type = ssType::pkt_leave;
		msg->client_socket = this->token->clientSocket;
		msg->room_num = this->roomNum;
		memcpy(msg->nickname, this->nickname.c_str(), sizeof(msg->nickname));

		if (listen_server.the_other_sock == NULL)
		{
			connect_server._send((char *)msg, sizeof(ss_leave));
		}
		else
		{
			listen_server._send((char *)msg, sizeof(ss_leave));
		}
		roomManager.leaveRoom(this, this->roomNum);

		printf("remove in room...\n");
	}

	if (token->clientSocket != NULL){
		char* sendMsg = poolManager.pop();
		ss_disconnect* tmpDisconnect = (ss_disconnect*)sendMsg;

		tmpDisconnect->type = ssType::pkt_disconnect;
		tmpDisconnect->client_socket = this->token->clientSocket;

		if (listen_server.the_other_sock == NULL)
		{
			connect_server._send((char *)tmpDisconnect, sizeof(ss_disconnect));
		}
		else{
			listen_server._send((char *)tmpDisconnect, sizeof(ss_disconnect));
		}
	}
	/* remove in list */
	g_vPlayers.remove(this);
}