#include "RoomManager.h"
#include "Client_Protocol.h"

extern RoomManager roomManager;

void RoomManager::printInfo(){
	std::list<Room*>::iterator iter;
	printf("\n< Current Room List >\n");
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		printf("Room %d : %d persons are coonnecting...\n", (*iter)->roomNumber, (*iter)->players.size());
	}
	printf("\n");
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

	// nickname check
	std::list<Room*>::iterator iter = roomManager.rooms.begin();
	for (; iter != roomManager.rooms.end(); ++iter)
	{
		std::list<Player*>::iterator iter2 = (*iter)->players.begin();
		for (; iter2 != (*iter)->players.end(); ++iter2)
		{
			if (!strcmp((*iter2)->nickname.c_str(), p->nickname.c_str()))
			{
				return fail_signal::fs_alreadyexist;
			}
		}
	}

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

bool RoomManager::leaveRoom(Player* p, int roomNumber)
{
	Room* room = findRoom(roomNumber);
	if (room == NULL){
		printf("No ROOM!\n");
		return false;
	}
	room->playerQuit(p, true);

	printInfo();

	return true;
}