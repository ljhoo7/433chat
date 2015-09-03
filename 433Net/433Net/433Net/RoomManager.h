#pragma once
#include "utilities.h"
#include "Player.h"
#include "room.h"


class RoomManager{
public:
	std::list<Room*> rooms;

public:
	void createRoom(int roomNumber);
	void destroyRoom(int roomNumber);
	Room* findRoom(int roomNumber);
	void leaveRoom(Player* p, int roomNumber);
	void enterRoom(Player* p, int roomNumber);
	void printInfo();


};