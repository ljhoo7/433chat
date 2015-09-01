#include "Player.h"
#include "RoomManager.h"

extern RoomManager roomManager;

extern SOCKET the_other_sock;

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

			// 상대 서버와 연결이 되어있다면 챗팅 메세지를 보내준다.
			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);

			break;
		}

		case pkt_type::pt_join:
		{	
			pkt.m_join.nickname[size - 8] = '\0';
			this->nickname = pkt.m_join.nickname;
			roomManager.enterRoom(this, pkt.m_join.room_num);

			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);
			break;
		}
		case pkt_type::pt_create:
		{	
			roomManager.createRoom(pkt.m_create.room_num);
// 연동되고 있는 서버가 한 대 더 있다면 방 생성 메세지를 보내준다.
			if (the_other_sock != NULL)
			{
				send(the_other_sock, buf, size, 0);
			}			break;
		}
		case pkt_type::pt_leave:
		{
			pkt.m_join.nickname[size - 8] = '\0';
			this->nickname = pkt.m_leave.nickname;
			roomManager.leaveRoom(this, this->roomNum);

			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);
			break;
		}
		case pkt_type::pt_destroy:
		{	
			roomManager.destroyRoom(pkt.m_destroy.room_num);

			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);
			break;
		}
	}
}

void Player::remove(){
	Room* room = roomManager.findRoom(this->roomNum);
	if (room == NULL){
		printf("No ROOM!\n");
		return;
	}
	if (room != NULL) room->playerQuit(this, true);

	printf("remove in room...\n");
}