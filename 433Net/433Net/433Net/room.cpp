#include "RoomManager.h"
#include "Client_Protocol.h"

extern RoomManager roomManager;

extern SOCKET the_other_sock;

Room::Room(int roomNumber){
	this->roomNumber = roomNumber;
}
Room::~Room(){
	std::list<Player*>::iterator iter;
	for (iter = players.begin(); iter != players.end(); iter++){
		playerQuit(*iter, false);
	}
}

void Room::playerEnter(Player* player){
	player->roomNum = roomNumber;

	t_join packet;

	packet.type = pkt_type::pt_join;
	packet.room_num = this->roomNumber;

	memcpy(packet.nickname, (player->nickname).c_str(), (player->nickname).size());

	broadcast_msg((char *)&packet, (player->nickname).size() + 8);

	/* �����ߴٰ� �˸��� */
	players.push_back(player);
}


void Room::playerQuit(Player* player, bool msg){
	player->roomNum = -1;
	players.remove(player);

	t_leave packet;
	int size = (player->nickname).size() + 8;
	packet.room_num = player->roomNum;
	packet.type = pkt_type::pt_leave;
	packet.token = player->identifier;

	memcpy(packet.nickname, (player->nickname).c_str(), (player->nickname).size());

	if (msg) broadcast_msg((char *)&packet, size);

	/* �����ٰ� �˸��� */
}

void Room::broadcast_msg(char* msg, int size){
	std::list<Player*>::iterator iter;
	//printf("%d ��ȣ �濡 %d�� ������\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++){
		(*iter)->send_msg(msg, size);
	}
}