#include "player.h"
#include "packet.h"
#include "room.h"
#include "RoomManager.h"
#include "protocol.h"
#include "InterServer.h"
#include "Client_Protocol.h"
#include "reciever.h"

extern RoomManager roomManager;
extern LogicHandle logicHandle;

extern InterServer listen_server;
extern InterServer connect_server;

extern Reciever receiver;

extern int identifier_seed;

Player::Player() : poolManager(10), packetPoolManager(10)
{
	identifier = identifier_seed++;
}

bool Player::recieveProcess()
{
	if (token.clientSocket == NULL)
	{
		std::cout << "There is a weired UserToken which has note connecd socket." << std::endl;
		return false;
	}
	char* buf = this->token.buf;

	if (this->token.position < HEADER_SIZE){
		if (token.position == 0){
			token.remainBytes = HEADER_SIZE;
		}
		return recieve(buf + token.position, token.remainBytes);
	}
	else{
		if (token.position == HEADER_SIZE){
			token.var = false;
			pkt_type _type = (pkt_type)((unsigned short)(*buf));
			switch (_type)
			{
			case pkt_type::pt_create:
				token.remainBytes = sizeof(t_create)-2;
				std::cout << "pt_create" << std::endl;
				break;
			case pkt_type::pt_destroy:
				token.remainBytes = sizeof(t_destroy)-2;
				std::cout << "pt_destroy" << std::endl;
				break;
			case pkt_type::pt_join:
				token.remainBytes = sizeof(t_join)-2;
				std::cout << "pt_join" << std::endl;
				break;
			case pkt_type::pt_leave:
				token.remainBytes = sizeof(t_leave)-2;
				std::cout << "pt_leave" << std::endl;
				break;
			case pkt_type::pt_chat:
				token.var = true;
				token.remainBytes = sizeof(short);
				std::cout << "pt_chat" << std::endl;
				break;
			default:
				disconnect();
				return false;
			}
		}
		bool check = recieve(buf + token.position, token.remainBytes);
		if (token.remainBytes <= 0){
			if (token.var){
				if (this->token.position == HEADER_SIZE * 2){
					memcpy(&token.remainBytes, buf + HEADER_SIZE, sizeof(short));
					token.remainBytes -= HEADER_SIZE;
				}
				token.var = false;
			}
			else{
				token.position = 0;
				char* buf = poolManager.pop();
				Packet* msg = packetPoolManager.pop();
				msg->type = 2;
				msg->owner = &this->token;
				msg->msg = buf;
				memcpy(buf, token.buf, sizeof(BUFSIZE));
				logicHandle.enqueue_oper(msg);
			}
		}
		return check;
	}
	return true;
}

bool Player::recieve(char* buf, int size){
	if (size == 0) return true;
	int ret = recv(token.clientSocket, buf, size, 0);
	if (ret == SOCKET_ERROR){
		printf("recieve error\n");
		disconnect();
		return false;
	}
	token.position += ret;
	token.remainBytes -= ret;

	return true;
}

void Player::disconnect(){
	if (token.clientSocket != NULL){
		printf("closed a connection with a client.\n");
		closesocket(token.clientSocket);
		token.clientSocket = NULL;
	}
}

void Player::send_msg(char *buf, int size)
{
	if (send(token.clientSocket, (char *)buf, size, 0) == SOCKET_ERROR)
	{
		std::cout << "Send() error" << std::endl;
	}
}

void Player::recieve_msg(char* buf, int size)
{
}

void Player::packetHandling(Packet *packet)
{
	if (token.clientSocket == NULL)
	{
		return;
	}

	pkt_type _type = (pkt_type)(*packet->msg);

	t_create			tmpSsCreate;
	t_destroy			tmpSsDestroy;
	t_join				tmpSsJoin;
	t_leave			tmpSsLeave;
	char				*tmpCsChat;

	t_create_success	tmpCsCreateSuccess;
	t_create_failure	tmpCsCreateFailure;
	t_destroy_success	tmpCsDestroySuccess;
	t_destroy_failure	tmpCsDestroyFailure;
	t_join_success		tmpCsJoinSuccess;
	t_join_failure		tmpCsJoinFailure;
	t_leave_success		tmpCsLeaveSuccess;

	int result;

	switch (_type)
	{
	case pkt_type::pt_create:
		memcpy(&tmpSsCreate, packet->msg, sizeof(t_create));
		result = roomManager.createRoom(tmpSsCreate.room_num);
		if (result == -1)
		{
			tmpSsCreate.type = ssType::pkt_create;
			tmpSsCreate.room_num = this->roomNum;
			//------------------------------------------------------------------------

			tmpCsCreateSuccess.type = pkt_type::pt_create_success;

			send_msg((char *)&tmpCsCreateSuccess, sizeof(t_create_success));
		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpCsCreateFailure.type = pkt_type::pt_create_failure;
			tmpCsCreateFailure.fail_signal = fail_signal::fs_overflow;

			send_msg((char *)&tmpCsCreateFailure, sizeof(t_create_failure));
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpCsCreateFailure.type = pkt_type::pt_create_failure;
			tmpCsCreateFailure.fail_signal = fail_signal::fs_alreadyexist;

			send_msg((char *)&tmpCsCreateFailure, sizeof(t_create_failure));
		}
		break;
	case pkt_type::pt_destroy:
		memcpy(&tmpSsDestroy, packet->msg, sizeof(t_destroy));
		result = roomManager.destroyRoom(tmpSsDestroy.room_num);
		if (result == -1)
		{
			tmpSsDestroy.type = ssType::pkt_destroy;
			tmpSsDestroy.room_num = this->roomNum;

			//------------------------------------------------------------------------

			tmpCsDestroySuccess.type = pkt_type::pt_destroy_success;

			send_msg((char *)&tmpCsDestroyFailure, sizeof(t_destroy_failure));
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpCsDestroyFailure.type = pkt_type::pt_destroy_failure;
			tmpCsDestroyFailure.fail_signal = fail_signal::fs_no_exist;

			send_msg((char *)&tmpCsDestroyFailure, sizeof(t_destroy_failure));
		}
		break;
	case pkt_type::pt_join:
		memcpy(&tmpSsJoin, packet->msg, sizeof(t_join));
		result = roomManager.enterRoom(this, tmpSsJoin.room_num);
		if (result == -1)
		{
			tmpSsJoin.type = ssType::pkt_join;
			tmpSsJoin.room_num = this->roomNum;

			//------------------------------------------------------------------------

			tmpCsJoinSuccess.type = pkt_type::pt_join_success;
			tmpCsJoinSuccess.token = this->identifier;

			send_msg((char *)&tmpCsJoinSuccess, sizeof(t_join_success));
		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpCsJoinFailure.type = pkt_type::pt_create_failure;
			tmpCsJoinFailure.fail_signal = fail_signal::fs_overflow;

			send_msg((char *)&tmpCsJoinFailure, sizeof(t_join_failure));
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpCsJoinFailure.type = pkt_type::pt_join_failure;
			tmpCsJoinFailure.fail_signal = fail_signal::fs_alreadyexist;

			send_msg((char *)&tmpCsJoinFailure, sizeof(t_join_failure));
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpCsJoinFailure.type = pkt_type::pt_join_failure;
			tmpCsJoinFailure.fail_signal = fail_signal::fs_no_exist;

			send_msg((char *)&tmpCsJoinFailure, sizeof(t_join_failure));
		}
		break;
	case pkt_type::pt_leave:
		memcpy(&tmpSsJoin, packet->msg, sizeof(t_join));
		result = roomManager.enterRoom(this, tmpSsJoin.room_num);
		if (result == true)
		{
			tmpSsJoin.type = ssType::pkt_join;
			tmpSsJoin.room_num = this->roomNum;
			//------------------------------------------------------------------------

			tmpCsLeaveSuccess.type = pkt_type::pt_leave_success;

			send_msg((char *)&tmpCsLeaveSuccess, sizeof(t_leave_success));

			//-------------------------------------------------------------------------
		}
		else
		{
			std::cout << "The Leave Command has been failed. This is very extra ordinary event !" << std::endl;
		}
		break;
	case pkt_type::pt_chat:
		unsigned short size;
		unsigned short type = pkt_type::pt_chat_alarm;
		memcpy(packet->msg, &type, sizeof(unsigned short));
		memcpy(&size, packet->msg + 2, sizeof(unsigned short));
		tmpCsChat = new char[size];
		memcpy(tmpCsChat, packet->msg, size);

		roomManager.findRoom(this->roomNum)->broadcast_msg(tmpCsChat, size);
		break;
	}
}

void Player::remove()
{
	Room* room = roomManager.findRoom(this->roomNum);
	if (room == NULL){
		printf("No ROOM!\n");
		return;
	}
	if (token.clientSocket != NULL){
		ss_disconnect tmpDisconnect;

		tmpDisconnect.type = ssType::pkt_disconnect;
		tmpDisconnect.client_socket = this->token.clientSocket;
		
		//-----------------------------------------------

		ss_leave msg;

		msg.type = ssType::pkt_leave;
		msg.client_socket = this->token.clientSocket;
		msg.room_num = this->roomNum;

		memcpy(msg.nickname, this->nickname.c_str(), this->nickname.size());

		if (listen_server.the_other_sock == NULL)
		{
			connect_server._send((char *)&msg, sizeof(ss_leave));
			connect_server._send((char *)&tmpDisconnect, sizeof(ss_disconnect));
		}
		else
		{
			listen_server._send((char *)&msg, sizeof(ss_leave));
			listen_server._send((char *)&tmpDisconnect, sizeof(ss_disconnect));
		}

		//-----------------------------------------------

		t_leave cMsg;

		cMsg.type = pkt_type::pt_leave;
		cMsg.room_num = this->roomNum;
		cMsg.token = this->identifier;

		memcpy(cMsg.nickname, this->nickname.c_str(), this->nickname.size());

		roomManager.findRoom(this->roomNum)->broadcast_msg((char *)&cMsg, sizeof(t_leave));
	}

	if (room != NULL) room->playerQuit(this, true);

	printf("remove in room...\n");
}