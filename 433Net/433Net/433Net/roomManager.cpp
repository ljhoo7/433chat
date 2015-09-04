#include "RoomManager.h"
#include "Client_Protocol.h"

extern RoomManager roomManager;

void RoomManager::printInfo(){
	std::list<Room*>::iterator iter;
	printf("\n\n현재 방 목록\n");
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		printf("방 %d에 %d명 접속 중\n", (*iter)->roomNumber, (*iter)->players.size());
	}
	printf("\n");
}

bool RoomManager::checkExist(int roomNumber)
{
	std::list<Room*>::iterator iter;
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		if (roomNumber == (*iter)->roomNumber)
		{
			return true;
		}
	}
	return false;
}

int RoomManager::createRoom(int roomNumber){
	if (rooms.size() > ROOM_MAX)
	{
		return fail_signal::fs_overflow;
	}
	else
	{
		std::list<Room*>::iterator iter;
		for (iter = rooms.begin(); iter != rooms.end(); ++iter){
			if ((*iter)->roomNumber < roomNumber){
				continue;
			}
			else if ((*iter)->roomNumber == roomNumber){
				return fail_signal::fs_alreadyexist;
			}
			else{
				break;
			}
		}
		Room* room = new Room(roomNumber);
		//enterRoom(p, roomNumber);
		rooms.insert(iter, room);

		printInfo();

		return -1;
	}
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

int RoomManager::enterRoom(Player* p, int roomNumber){
	Room* room = findRoom(roomNumber);
	if (room == NULL)
		return fail_signal::fs_no_exist;
	if (room->players.size() > PLAYER_MAX)
		return fail_signal::fs_overflow;

	room->playerEnter(p);

	printInfo();

	return -1;
}

int RoomManager::destroyRoom(int roomNumber){
	Room* room = findRoom(roomNumber);
	if (room == NULL)
		return fail_signal::fs_no_exist;
	rooms.remove(room);

	delete room;

	printInfo();

	return -1;
}

bool RoomManager::leaveRoom(Player* p, int roomNumber){
	Room* room = findRoom(roomNumber);
	if (room == NULL){
		printf("No ROOM!\n");
		return false;
	}
	room->playerQuit(p, true);

	printInfo();

	return true;
}