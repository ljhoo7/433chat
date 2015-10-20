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
	PRINTF("< Current Room List >\n");
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		PRINTF("Room %d : %d persons are connecting...\n", (*iter)->roomNumber, (*iter)->players.size());
	}
	PRINTF("\n");
}

int CRoomManager::CreateRoom(int roomNumber){
	if (rooms.size() > ROOM_MAX)
	{
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
				return fail_signal::fs_alreadyexist;
			}
			else{
				break;
			}
		}
		CRoom* room = new CRoom(roomNumber);
		//enterRoom(p, roomNumber);
		rooms.insert(iter, room);

		PrintInfo();

		return -1;
	}
}

CRoom* CRoomManager::FindRoom(int roomNumber){
	if (roomNumber == -1) return NULL;

	std::list<CRoom*>::iterator iter;
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		if ((*iter)->roomNumber == roomNumber){
			return (*iter);
		}
	}

	return NULL;
}

int CRoomManager::EnterRoom(CPlayer* p, int roomNumber){
	CRoom* room = FindRoom(roomNumber);
	if (room == NULL)
		return fail_signal::fs_no_exist;
	if (room->players.size() > PLAYER_MAX)
		return fail_signal::fs_overflow;

	// nickname check
	std::list<CRoom*>::iterator iter = chatServer->roomManager.rooms.begin();
	for (; iter != chatServer->roomManager.rooms.end(); ++iter)
	{
		
		std::list<CPlayer*>::iterator iter2 = (*iter)->players.begin();
		for (; iter2 != (*iter)->players.end(); ++iter2)
		{
			if (!strcmp((*iter2)->nickname.c_str(), p->nickname.c_str()))
			{
				return fail_signal::fs_alreadyexist;
			}
		}
	}

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
	t_kick tmpKick;

	tmpKick.type = pkt_type::pt_kick;

	room->BroadcastMsg((char*)&tmpKick, sizeof(t_kick));

	for (std::list<CPlayer*>::iterator iter = room->players.begin();
		iter != room->players.end(); ++iter)
	{
		(*iter)->roomNum = -1;
	}

	rooms.remove(room);

	PrintInfo();

	return -1;
}

bool CRoomManager::LeaveRoom(CPlayer* p, int roomNumber)
{
	CRoom* room = FindRoom(roomNumber);
	if (room == NULL){
		PRINTF("No ROOM!\n");
		return false;
	}
	room->PlayerQuit(p, true);

	PrintInfo();

	return true;
}