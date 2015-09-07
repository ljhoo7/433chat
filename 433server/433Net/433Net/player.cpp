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
extern std::list<Player*> g_vPlayers;

Player::Player(bool isMine) : poolManager(10), packetPoolManager(10)
{
	this->isMine = isMine;
	this->nickname = "";
	this->roomNum = -1;
	this->identifier = identifier_seed++;
}

bool Player::recieveProcess()
{
	if (token.clientSocket == NULL)
	{
		std::cout << "There is a weired UserToken which has note connecd socket." << std::endl;
		return false;
	}
	char* buf = this->token.buf;

	if (this->token.position < HEADER_SIZE)
	{
		if (token.position == 0){
			token.remainBytes = HEADER_SIZE;
		}
		return recieve(buf + token.position, token.remainBytes);
	}
	else
	{
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
			if (token.var)
			{
				int typePlusLength = HEADER_SIZE << 1;
				if (this->token.position == typePlusLength){
					memcpy(&token.remainBytes, buf + HEADER_SIZE, sizeof(short));
					token.remainBytes -= typePlusLength;
				}
				token.var = false;
			}
			else
			{
				token.position = 0;
				char* buf = poolManager.pop();
				Packet* msg = packetPoolManager.pop();
				msg->type = 2;
				msg->owner = &this->token;
				msg->msg = buf;
				memcpy(buf, token.buf, BUFSIZE);
				logicHandle.enqueue_oper(msg, false);
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

bool Player::valid_Packet(Packet *packet){
	if (!this->isMine) return false;

	pkt_type _type = (pkt_type)(*packet->msg);
	
	t_chat tmpChat;
	t_leave tmpLeave;

	int size;

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

void Player::playerSync(char *buf, int size){
	if (listen_server.the_other_sock != NULL){
		listen_server._send(buf, size);
	}
	if (connect_server.the_other_sock != NULL){
		connect_server._send(buf, size);
	}
}

void Player::packetHandling(Packet *packet)
{
	if (token.clientSocket == NULL)
	{
		return;
	}

	if (!valid_Packet(packet)) return;

	pkt_type _type = (pkt_type)(*packet->msg);

	t_create			tmpCreate;
	t_destroy			tmpDestroy;
	t_join				tmpJoin;
	t_leave				tmpLeave;
	t_join_alarm		tmpJoinAlarm;
	t_leave_alarm		tmpLeaveAlarm;
	t_create_success	tmpCreateSuccess;
	t_create_failure	tmpCreateFailure;
	t_destroy_success	tmpDestroySuccess;
	t_destroy_failure	tmpDestroyFailure;
	t_join_success		tmpJoinSuccess;
	t_join_failure		tmpJoinFailure;
	t_leave_success		tmpLeaveSuccess;

	char				*tmpChat;
	t_chat_alarm		tmpChatAlarm;

	unsigned short		size, type;

	int result;

	switch (_type)
	{
	case pkt_type::pt_create:
		memcpy(&tmpCreate, packet->msg, sizeof(t_create));
		result = roomManager.createRoom(tmpCreate.room_num);
		if (result == -1)
		{
			ss_create msg;
			msg.type = ssType::pkt_create;
			msg.client_socket = token.clientSocket;
			msg.room_num = tmpCreate.room_num;
			playerSync((char *)&msg, sizeof(msg));
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
			std::cout << "create buffer overflow message has been sent." << std::endl;
		}
		else if (result == fail_signal::fs_alreadyexist)
		{
			tmpCreateFailure.type = pkt_type::pt_create_failure;
			tmpCreateFailure.fail_signal = fail_signal::fs_alreadyexist;

			send_msg((char *)&tmpCreateFailure, sizeof(t_create_failure));
			std::cout << "create buffer already exist message has been sent." << std::endl;
		}
		break;
	case pkt_type::pt_destroy:
		memcpy(&tmpDestroy, packet->msg, sizeof(t_destroy));
		result = roomManager.destroyRoom(tmpDestroy.room_num);
		if (result == -1)
		{
			ss_destroy msg;
			msg.type = ssType::pkt_destroy;
			msg.client_socket = token.clientSocket;
			msg.room_num = tmpDestroy.room_num;
			playerSync((char *)&msg, sizeof(msg));
			
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
		memcpy(&tmpJoin, packet->msg, sizeof(t_join));
		result = roomManager.enterRoom(this, tmpJoin.room_num);
		if (result == -1)
		{
			ss_join msg;
			msg.type = ssType::pkt_join;
			msg.client_socket = token.clientSocket;
			msg.room_num = tmpJoin.room_num;
			memcpy(msg.nickname, tmpJoin.nickname, sizeof(msg.nickname));
			playerSync((char *)&msg, sizeof(msg));

			std::cout << "join message has been sent." << std::endl;
			
			//------------------------------------------------------------------------

			tmpJoinSuccess.type = pkt_type::pt_join_success;
			tmpJoinSuccess.token = this->identifier;

			send_msg((char *)&tmpJoinSuccess, sizeof(t_join_success));
			std::cout << "join success message has been sent." << std::endl;

			memcpy(tmpJoinAlarm.nickname, tmpJoin.nickname, 20);
			tmpJoinAlarm.room_num = tmpJoin.room_num;
			tmpJoinAlarm.type = pkt_type::pt_join_alarm;

			roomManager.findRoom(this->roomNum)->broadcast_msg((char*)&tmpJoinAlarm, sizeof(t_join_alarm));
			std::cout << "join alarm message has been sent." << std::endl;
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
		memcpy(&tmpLeave, packet->msg, sizeof(t_leave));
		result = roomManager.leaveRoom(this, tmpLeave.room_num);

		if (result == true)
		{
			ss_leave msg;
			msg.type = ssType::pkt_leave;
			msg.client_socket = token.clientSocket;
			msg.room_num = tmpLeave.room_num;
			msg.token = tmpLeave.token;
			memcpy(msg.nickname, tmpLeave.nickname, sizeof(msg.nickname));
			playerSync((char *)&msg, sizeof(msg));
			
			//------------------------------------------------------------------------
			tmpLeaveSuccess.type = pkt_type::pt_leave_success;
			send_msg((char *)&tmpLeaveSuccess, sizeof(t_leave_success));

			std::cout << "leave success message has been sent." << std::endl;

			memcpy(tmpLeaveAlarm.nickname, tmpLeave.nickname, 20);
			tmpLeaveAlarm.room_num = tmpLeave.room_num;
			tmpLeaveAlarm.type = pkt_type::pt_leave_alarm;
			roomManager.findRoom(this->roomNum)->broadcast_msg((char*)&tmpLeaveAlarm, sizeof(t_leave_alarm));
			
			std::cout << "leave alarm message has been sent." << std::endl;
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

		std::cout << "chat alarm message has been sent." << std::endl;		break;
	}

	this->packetPoolManager.push(packet);
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


	/* remove in list */
	g_vPlayers.remove(this);

	printf("remove in room...\n");
}