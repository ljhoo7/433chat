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
			// ��� ������ ������ �Ǿ��ִٸ� ê�� �޼����� �����ش�.
			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);

			//pkt.m_chat.str[size - 8] = '\0';
			//printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", size);
			//printf("[���� ������] %s\n", pkt.m_chat.str);
			Room* room = roomManager.findRoom(pkt.m_chat.room_num);
			if (room != NULL) room->broadcast_msg(buf, size);

			break;
		}

		case pkt_type::pt_join:
		{	
			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);

			pkt.m_join.nickname[size - 8] = '\0';
			this->nickname = pkt.m_join.nickname;
			roomManager.enterRoom(this, pkt.m_join.room_num);

			break;
		}
		case pkt_type::pt_create:
		{	
			if (the_other_sock != NULL)
			{
				send(the_other_sock, buf, size, 0);
			}

			roomManager.createRoom(pkt.m_create.room_num);
// �����ǰ� �ִ� ������ �� �� �� �ִٸ� �� ���� �޼����� �����ش�.
			
			break;
		}
		case pkt_type::pt_leave:
		{
			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);

			pkt.m_join.nickname[size - 8] = '\0';
			this->nickname = pkt.m_leave.nickname;
			roomManager.leaveRoom(this, this->roomNum);

			break;
		}
		case pkt_type::pt_destroy:
		{	
			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, size, 0);

			roomManager.destroyRoom(pkt.m_destroy.room_num);

			break;
		}
	}
}

void Player::remove(){
	if (the_other_sock != NULL){
		t_leave msg;
		msg.length = sizeof(msg.length) + sizeof(msg.room_num)
			+ sizeof(msg.type) + this->nickname.size();
		msg.room_num = this->roomNum;
		msg.type = pkt_type::pt_leave;
		memcpy(msg.nickname, this->nickname.c_str(), this->nickname.size());
		send(the_other_sock, (char*)&msg, msg.length, 0);
	}

	Room* room = roomManager.findRoom(this->roomNum);
	if (room == NULL){
		printf("No ROOM!\n");
		return;
	}
	
	

	if (room != NULL) room->playerQuit(this, true);

	printf("remove in room...\n");
}