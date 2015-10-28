#pragma once

class CRoomManager
{
public:
	CRoomManager();
	~CRoomManager();

	int CreateRoom(int roomNumber);
	int DestroyRoom(int roomNumber);
	CRoom* FindRoom(int roomNumber);
	bool LeaveRoom(CPlayer* p, int roomNumber);
	int EnterRoom(CPlayer* p, int roomNumber);
	void PrintInfo();

	void TotalRoomInfo(sag_total_room_info* msg, int* size);
	void TotalRoomInfo(char* buf, int* position, unsigned short* roomCnt);
	int GetRoomCnt();

private:
	std::list<CRoom*> rooms;
	CRITICAL_SECTION roomLock;
};

