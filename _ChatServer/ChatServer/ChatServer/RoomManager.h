#pragma once

class CRoomManager
{
public:
	CRoomManager();
	~CRoomManager();

	std::list<CRoom*> rooms;
	CRITICAL_SECTION roomLock;

	int CreateRoom(int roomNumber);
	int DestroyRoom(int roomNumber);
	CRoom* FindRoom(int roomNumber);
	bool LeaveRoom(CPlayer* p, int roomNumber);
	int EnterRoom(CPlayer* p, int roomNumber);
	void PrintInfo();
};

