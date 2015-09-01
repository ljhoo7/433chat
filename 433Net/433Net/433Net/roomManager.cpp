#include "RoomManager.h"

void RoomManager::printInfo(){
	std::list<Room*>::iterator iter;
	printf("\n\n현재 방 목록\n");
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		printf("방 %d에 %d명 접속 중\n", (*iter)->roomNumber, (*iter)->players.size());
	}
	printf("\n");
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
	if (roomNumber == -1) return NULL;

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
	if (room == NULL){
		printf("No ROOM!\n");
		return;
	}
	room->playerQuit(p, true);

	printInfo();
}