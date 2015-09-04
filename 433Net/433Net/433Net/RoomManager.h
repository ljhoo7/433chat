#pragma once
#include "utilities.h"
#include "Player.h"
#include "room.h"

class RoomManager{
public:
	std::list<Room*> rooms;
public:
	int createRoom(int roomNumber);
	int destroyRoom(int roomNumber);
	Room* findRoom(int roomNumber);
	bool leaveRoom(Player* p, int roomNumber);
	int enterRoom(Player* p, int roomNumber);
	void printInfo();
};