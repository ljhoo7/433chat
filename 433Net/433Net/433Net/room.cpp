#include "RoomManager.h"

extern RoomManager roomManager;

extern SOCKET the_other_sock;

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
	packet.length = sizeof(packet);
	packet.type = pkt_type::pt_leave;
	packet.room_num = this->roomNumber;
	memcpy(packet.nickname, (player->nickname).c_str(), (player->nickname).size());
	broadcast_msg((char *)&packet, sizeof(packet));

	/* 나갔다고 알리기 */
}

void Room::broadcast_msg(char* msg, int size){
	std::list<Player*>::iterator iter;
	//printf("%d 번호 방에 %d명 접속중\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++){
		(*iter)->send_msg(msg, size);
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

	// 연동되고 있는 서버가 한 대 더 있다면 방 생성 메세지를 보내준다.
	if (the_other_sock != NULL)
	{
		t_create result_pkt;
		result_pkt.length = sizeof(t_create);
		result_pkt.type = pkt_type::pt_create;
		result_pkt.room_num = roomNumber;
		
		send(the_other_sock, (char*)&result_pkt, sizeof(t_packet), 0);
	}

	//enterRoom(p, roomNumber);
	rooms.insert(iter, room);
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
}

void RoomManager::destroyRoom(int roomNumber){
	Room* room = findRoom(roomNumber);
	rooms.remove(room);

	delete room;
}

void RoomManager::leaveRoom(Player* p, int roomNumber){
	Room* room = findRoom(roomNumber);
	if (room == NULL) return;
	room->playerQuit(p);

	delete room;
}