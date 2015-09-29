#pragma once

class CRoomManager
{
public:
	CRoomManager();
	~CRoomManager();

	std::list<CRoom*> rooms;

	int createRoom(int roomNumber);
	int destroyRoom(int roomNumber);
	CRoom* findRoom(int roomNumber);
	bool leaveRoom(CPlayer* p, int roomNumber);
	int enterRoom(CPlayer* p, int roomNumber);
	void printInfo();
};

