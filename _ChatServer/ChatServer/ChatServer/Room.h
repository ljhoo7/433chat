#pragma once

class CRoom
{
public:
	CRoom(int roomNumber);
	~CRoom();

	void playerEnter(CPlayer* player);
	void playerQuit(CPlayer* player, bool msg);
	void broadcast_msg(char* msg, int size);

	int roomNumber;
	std::list<CPlayer*> players;
};

