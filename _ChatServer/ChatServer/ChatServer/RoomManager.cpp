#include "stdafx.h"

extern ChatServer* chatServer;

CRoomManager::CRoomManager()
{
	InitializeCriticalSectionAndSpinCount(&roomLock, 4000);
}

CRoomManager::~CRoomManager()
{
	DeleteCriticalSection(&roomLock);
}

void CRoomManager::PrintInfo(){
	std::list<CRoom*>::iterator iter;
	PRINT("< Current Room List >\n");

	EnterCriticalSection(&roomLock);
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		PRINT("Room %d : %d persons are connecting...\n", (*iter)->roomNumber, (*iter)->GetPlayerSize());
	}
	LeaveCriticalSection(&roomLock);

	PRINT("\n");
}

int CRoomManager::CreateRoom(int roomNumber){
	EnterCriticalSection(&roomLock);
	if (rooms.size() > ROOM_MAX)
	{
		LeaveCriticalSection(&roomLock);
		return fail_signal::fs_overflow;
	}
	else
	{
		std::list<CRoom*>::iterator iter;
		for (iter = rooms.begin(); iter != rooms.end(); ++iter){
			if ((*iter)->roomNumber < roomNumber){
				continue;
			}
			else if ((*iter)->roomNumber == roomNumber){
				LeaveCriticalSection(&roomLock);
				return fail_signal::fs_alreadyexist;
			}
			else{
				break;
			}
		}
		CRoom* room = new CRoom(roomNumber);
		//enterRoom(p, roomNumber);
		rooms.insert(iter, room);
		LeaveCriticalSection(&roomLock);

		PrintInfo();

		return -1;
	}
}

CRoom* CRoomManager::FindRoom(int roomNumber){
	if (roomNumber == -1) return NULL;

	std::list<CRoom*>::iterator iter;

	CRoom* ret = NULL;

	//PRINT("find room call!!\n");
	EnterCriticalSection(&roomLock);
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		if ((*iter)->roomNumber == roomNumber){
			ret = (*iter);
			break;
		}
	}
	LeaveCriticalSection(&roomLock);

	//PRINT("find room call2!!\n");
	return ret;
}

int CRoomManager::EnterRoom(CPlayer* p, int roomNumber){
	CRoom* room = FindRoom(roomNumber);
	if (room == NULL)
		return fail_signal::fs_no_exist;
	if (room->GetPlayerSize() > PLAYER_MAX)
		return fail_signal::fs_overflow;

	// nickname check

	EnterCriticalSection(&roomLock);
	std::list<CRoom*>::iterator iter = chatServer->roomManager.rooms.begin();
	for (; iter != chatServer->roomManager.rooms.end(); ++iter)
	{
		if (!room->NickNameCheck(p->nickname.c_str())) return fail_signal::fs_alreadyexist;
	}
	LeaveCriticalSection(&roomLock);

	room->PlayerEnter(p);
	

	PrintInfo();

	return -1;
}

int CRoomManager::DestroyRoom(int roomNumber)
{
	CRoom* room = FindRoom(roomNumber);
	if (room == NULL)
		return fail_signal::fs_no_exist;

	// Sending the kick message to all players in the room to be destroyed.

	room->KickAllPlayer();

	EnterCriticalSection(&roomLock);
	rooms.remove(room);
	LeaveCriticalSection(&roomLock);

	PrintInfo();

	return -1;
}

bool CRoomManager::LeaveRoom(CPlayer* p, int roomNumber)
{
	CRoom* room = FindRoom(roomNumber);

	if (room == NULL){
		PRINT("[RoomManager] No ROOM!\n");
		return false;
	}
	room->PlayerQuit(p, true);

	PrintInfo();

	return true;
}

void CRoomManager::TotalRoomInfo(sag_total_room_info* msg, int* size_)
{
	msg->type = sag_pkt_type::pt_total_room_info;

	EnterCriticalSection(&roomLock);
	msg->roomCnt = rooms.size();

	int i = 0;
	*size_ = sizeof(msg->type) + sizeof(msg->roomCnt);
	std::list<CRoom*>::iterator iter;
	for (iter = rooms.begin(); iter != rooms.end(); iter++)
	{
		CRoom *p = (*iter);
		msg->roomInfoList[i].roomNum = p->roomNumber;
		i++;

		*size_ += sizeof(SAGRoomInfo);
	}
	LeaveCriticalSection(&roomLock);
}

void CRoomManager::TotalRoomInfo(char* buf, int* position, unsigned short* roomCnt){
	EnterCriticalSection(&roomLock);
	std::list<CRoom*>::iterator iter;
	for (iter = rooms.begin(); iter != rooms.end(); iter++)
	{
		CRoom *p = (*iter);

		room_info info;
		info.room_num = p->roomNumber;

		memcpy(buf + *position, &info, sizeof(info));
		*position += sizeof(info);
	}
	*roomCnt = rooms.size();
	LeaveCriticalSection(&roomLock);
}

int CRoomManager::GetRoomCnt(){
	int ret = 0;
	EnterCriticalSection(&roomLock);
	ret = rooms.size();
	LeaveCriticalSection(&roomLock);
	return ret;
}