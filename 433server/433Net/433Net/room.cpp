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

	/* informing the entered */
	players.push_back(player);
}


void Room::playerQuit(Player* player, bool msg){
	player->roomNum = -1;
	player->nickname = "";
	players.remove(player);

	/* informing the exited */
}

void Room::broadcast_msg(char* msg, int size){
	std::list<Player*>::iterator iter;
	//printf("Room %d : %d persons are connecting...\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++){
		(*iter)->send_msg(msg, size);
	}
}