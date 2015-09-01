#pragma once
#include "433Net.h"
#include "Player.h"

class Room{
public:
	int roomNumber;
	std::list<Player*> players;

public:
	Room(int roomNumber);
	~Room();

	void playerEnter(Player* player);

	void playerQuit(Player* player);

	void broadcast_msg(char* msg, int size);


};