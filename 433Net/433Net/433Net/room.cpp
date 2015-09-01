#include "RoomManager.h"

extern RoomManager roomManager;

Room::Room(int roomNumber){
	this->roomNumber = roomNumber;
}
Room::~Room(){
	std::list<Player*>::iterator iter;
	for (iter = players.begin(); iter != players.end(); iter++){
		playerQuit(*iter);
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

void Room::playerQuit(Player* player){
	player->roomNum = -1;
	players.remove(player);

	t_leave packet;
	packet.length = (player->nickname).size() + 4;
	packet.type = pkt_type::pt_leave;
	memcpy(packet.nickname, (player->nickname).c_str(), (player->nickname).size());
	broadcast_msg((char *)&packet, (player->nickname).size() + 4);

	/* 나갔다고 알리기 */
}

void Room::broadcast_msg(char* msg, int size){
	std::list<Player*>::iterator iter;
	printf("%d 번호 방에 %d명 접속중\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++){
		(*iter)->send_msg(msg, size);
	}
}


void RoomManager::printInfo(){
	std::list<Room*>::iterator iter;
	printf("\n\n현재 방 목록\n");
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		printf("방 %d에 %d명 접속 중\n", (*iter)->roomNumber, (*iter)->players.size());
	}
}

void RoomManager::createRoom(int roomNumber){
	std::list<Room*>::iterator iter;
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		if ((*iter)->roomNumber < roomNumber){
			continue;
		}
		else if ((*iter)->roomNumber == roomNumber){
			return;
		}
		else{
			break;
		}
	}
	Room* room = new Room(roomNumber);
	//enterRoom(p, roomNumber);
	rooms.insert(iter, room);

	printInfo();

}

Room* RoomManager::findRoom(int roomNumber){
	std::list<Room*>::iterator iter;
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		if ((*iter)->roomNumber == roomNumber){
			return (*iter);
		}
	}
	return NULL;
}

void RoomManager::enterRoom(Player* p, int roomNumber){
	Room* room = findRoom(roomNumber);
	if (room == NULL) return;
	room->playerEnter(p);

	printInfo();
}

void RoomManager::destroyRoom(int roomNumber){
	Room* room = findRoom(roomNumber);
	rooms.remove(room);

	delete room;

	printInfo();
}

void RoomManager::leaveRoom(Player* p, int roomNumber){
	Room* room = findRoom(roomNumber);
	if (room == NULL) return;
	room->playerQuit(p);

	printInfo();
}