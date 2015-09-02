#include "RoomManager.h"

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
	packet.length = (player->nickname).size() + 8;
	packet.type = pkt_type::pt_join;
	packet.room_num = this->roomNumber;
	memcpy(packet.nickname, (player->nickname).c_str(), (player->nickname).size());

	broadcast_msg((char *)&packet, (player->nickname).size() + 8);

	
	/* 입장했다고 알리기 */
	players.push_back(player);
}


void Room::playerQuit(Player* player, bool msg){
	player->roomNum = -1;
	players.remove(player);

	t_leave packet;
	int size = (player->nickname).size() + 8;
	packet.length = size;
	packet.room_num = player->roomNum;
	packet.type = pkt_type::pt_leave;
	memcpy(packet.nickname, (player->nickname).c_str(), (player->nickname).size());

	if (msg) broadcast_msg((char *)&packet, size);

	/* 나갔다고 알리기 */
}

void Room::broadcast_msg(char* msg, int size){
	std::list<Player*>::iterator iter;
	//printf("%d 번호 방에 %d명 접속중\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++){
		(*iter)->send_msg(msg, size);
	}
}