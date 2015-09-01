#include "Player.h"
#include "RoomManager.h"

extern RoomManager roomManager;

Player::Player(){
	roomNum = -1;
}

void Player::send_msg(char *buf, int size){
	/* packet handling*/
	this->token.send_msg(buf, size);
}

void Player::recieve_msg(char* buf, int size){
	t_packet pkt;
	memcpy(&pkt, buf, size);
	switch (pkt.m_any.type)
	{
		case pkt_type::pt_chat:
		{
			//pkt.m_chat.str[size - 8] = '\0';
			printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", size);
			//printf("[받은 데이터] %s\n", pkt.m_chat.str);
			Room* room = roomManager.findRoom(pkt.m_chat.room_num);
			if (room != NULL) room->broadcast_msg(buf, size);

			break;
		}

		case pkt_type::pt_join:
		{	
			pkt.m_join.nickname[size - 8] = '\0';
			this->nickname = pkt.m_join.nickname;
			roomManager.enterRoom(this, pkt.m_join.room_num);
			break;
		}
		case pkt_type::pt_create:
		{	
			roomManager.createRoom(pkt.m_create.room_num);
			break;
		}
		case pkt_type::pt_leave:
		{
			pkt.m_join.nickname[size - 8] = '\0';
			this->nickname = pkt.m_leave.nickname;
			roomManager.leaveRoom(this, this->roomNum);
			break;
		}
		case pkt_type::pt_destroy:
		{	
			roomManager.destroyRoom(pkt.m_destroy.room_num);
			break;
		}
	}
}

void Player::remove(){
	Room* room = roomManager.findRoom(this->roomNum);
	room->playerQuit(this);

	printf("remove in room...\n");
}