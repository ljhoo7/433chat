#include "player.h"
#include "packet.h"
#include "room.h"
#include "RoomManager.h"
#include "protocol.h"
#include "InterServer.h"
#include "Client_Protocol.h"

extern RoomManager roomManager;
extern LogicHandle logicHandle;

extern InterServer listen_server;
extern InterServer connect_server;

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
	char* buf = poolManager.pop();
	recieve(buf, sizeof(short));

	pkt_type _type = (pkt_type)((unsigned short)(*buf));
	switch (_type)
	{
	case pkt_type::pt_create:
		recieve(buf + 2, sizeof(t_create)-2);
		std::cout << "pt_create" << std::endl;
		break;
	case pkt_type::pt_destroy:
		recieve(buf + 2, sizeof(t_destroy)-2);
		std::cout << "pt_destroy" << std::endl;
		break;
	case pkt_type::pt_join:
		recieve(buf + 2, sizeof(t_join)-2);
		std::cout << "pt_join" << std::endl;
		break;
	case pkt_type::pt_leave:
		recieve(buf + 2, sizeof(t_leave)-2);
		std::cout << "pt_leave" << std::endl;
		break;
	case pkt_type::pt_chat:
		recieve(buf + 2, sizeof(short));
		unsigned short size;
		memcpy(&size, buf + 2, sizeof(short));
		recieve(buf + 4, size);
		std::cout << "pt_chat" << std::endl;
		break;
	default:
		disconnect();
		return false;
	}

	Packet* msg = packetPoolManager.pop();
	msg->type = 2;
	msg->owner = &this->token;
	msg->msg = buf;

	logicHandle.enqueue_oper(msg);

	return true;
}

void Player::recieve(char* buf, int size){
	if (size == 0) return;
	int ret = recv(token.clientSocket, buf, size, 0);
	if (ret == SOCKET_ERROR){
		printf("recieve error\n");
		disconnect();
	}
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

	ss_create			tmpSsCreate;
	ss_destroy			tmpSsDestroy;
	ss_join				tmpSsJoin;
	ss_leave			tmpSsLeave;

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
		memcpy(&tmpSsCreate, packet->msg, sizeof(ss_create));
		result = roomManager.createRoom(tmpSsCreate.room_num);
		if (result == -1)
		{
			tmpSsCreate.type = ssType::pkt_create;
			tmpSsCreate.client_socket = this->token.clientSocket;
			tmpSsCreate.room_num = this->roomNum;

			if (listen_server.the_other_sock == NULL)
			{
				connect_server._send((char *)&tmpSsCreate, sizeof(ss_create));
			}
			else
			{
				listen_server._send((char *)&tmpSsCreate, sizeof(ss_create));
			}

			//------------------------------------------------------------------------

			tmpCsCreateSuccess.type = pkt_type::pt_create_success;

			send(token.clientSocket, (char *)&tmpCsCreateSuccess, sizeof(t_create_success), 0);
		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpCsCreateFailure.type = pkt_type::pt_create_failure;
			tmpCsCreateFailure.fail_signal = fail_signal::fs_overflow;

			send(token.clientSocket, (char *)&tmpCsCreateFailure, sizeof(t_create_failure), 0);
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpCsCreateFailure.type = pkt_type::pt_create_failure;
			tmpCsCreateFailure.fail_signal = fail_signal::fs_alreadyexist;

			send(token.clientSocket, (char *)&tmpCsCreateFailure, sizeof(t_create_failure), 0);
		}
		break;
	case pkt_type::pt_destroy:
		memcpy(&tmpSsDestroy, packet->msg, sizeof(ss_destroy));
		result = roomManager.destroyRoom(tmpSsDestroy.room_num);
		if (result == -1)
		{
			tmpSsDestroy.type = ssType::pkt_destroy;
			tmpSsDestroy.client_socket = this->token.clientSocket;
			tmpSsDestroy.room_num = this->roomNum;

			if (listen_server.the_other_sock == NULL)
			{
				connect_server._send((char *)&tmpSsDestroy, sizeof(ss_destroy));
			}
			else
			{
				listen_server._send((char *)&tmpSsDestroy, sizeof(ss_destroy));
			}

			//------------------------------------------------------------------------

			tmpCsDestroySuccess.type = pkt_type::pt_destroy_success;

			send(token.clientSocket, (char *)&tmpCsDestroySuccess, sizeof(t_destroy_success), 0);
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpCsDestroyFailure.type = pkt_type::pt_destroy_failure;
			tmpCsDestroyFailure.fail_signal = fail_signal::fs_no_exist;

			send(token.clientSocket, (char *)&tmpCsDestroyFailure, sizeof(t_destroy_failure), 0);
		}
		break;
	case pkt_type::pt_join:
		memcpy(&tmpSsJoin, packet->msg, sizeof(ss_join));
		result = roomManager.enterRoom(this, tmpSsJoin.room_num);
		if (result == -1)
		{
			tmpSsJoin.type = ssType::pkt_join;
			tmpSsJoin.client_socket = this->token.clientSocket;
			tmpSsJoin.room_num = this->roomNum;

			if (listen_server.the_other_sock == NULL)
			{
				connect_server._send((char *)&tmpSsJoin, sizeof(ss_join));
			}
			else
			{
				listen_server._send((char *)&tmpSsJoin, sizeof(ss_join));
			}

			//------------------------------------------------------------------------

			tmpCsJoinSuccess.type = pkt_type::pt_create_success;

			send(token.clientSocket, (char *)&tmpCsJoinSuccess, sizeof(t_create_success), 0);
		}
		else if (result == fail_signal::fs_overflow)
		{
			tmpCsJoinFailure.type = pkt_type::pt_create_failure;
			tmpCsJoinFailure.fail_signal = fail_signal::fs_overflow;

			send(token.clientSocket, (char *)&tmpCsJoinFailure, sizeof(t_create_failure), 0);
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpCsJoinFailure.type = pkt_type::pt_create_failure;
			tmpCsJoinFailure.fail_signal = fail_signal::fs_alreadyexist;

			send(token.clientSocket, (char *)&tmpCsJoinFailure, sizeof(t_create_failure), 0);
		}
		else if (result == fail_signal::fs_no_exist)
		{
			tmpCsJoinFailure.type = pkt_type::pt_create_failure;
			tmpCsJoinFailure.fail_signal = fail_signal::fs_no_exist;

			send(token.clientSocket, (char *)&tmpCsJoinFailure, sizeof(t_create_failure), 0);
		}
		break;
	case pkt_type::pt_leave:
		memcpy(&tmpSsJoin, packet->msg, sizeof(ss_join));
		result = roomManager.enterRoom(this, tmpSsJoin.room_num);
		if (result == -1)
		{
			tmpSsJoin.type = ssType::pkt_join;
			tmpSsJoin.client_socket = this->token.clientSocket;
			tmpSsJoin.room_num = this->roomNum;

			if (listen_server.the_other_sock == NULL)
			{
				connect_server._send((char *)&tmpSsJoin, sizeof(ss_join));
			}
			else
			{
				listen_server._send((char *)&tmpSsJoin, sizeof(ss_join));
			}

			//------------------------------------------------------------------------

			tmpCsJoinSuccess.type = pkt_type::pt_create_success;

			send(token.clientSocket, (char *)&tmpCsJoinSuccess, sizeof(t_create_success), 0);
		}
		break;
	case pkt_type::pt_chat:
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