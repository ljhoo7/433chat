#include "stdafx.h"

extern CRoomManager roomManager;

CRoomManager::CRoomManager()
{
}

CRoomManager::~CRoomManager()
{
}

void CRoomManager::printInfo(){
	std::list<CRoom*>::iterator iter;
	printf("< Current Room List >\n");
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		printf("Room %d : %d persons are coonnecting...\n", (*iter)->roomNumber, (*iter)->players.size());
	}
	printf("\n");
}

int CRoomManager::createRoom(int roomNumber){
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

		printInfo();

		return -1;
	}
}

CRoom* CRoomManager::findRoom(int roomNumber){
	if (roomNumber == -1) return NULL;

	std::list<CRoom*>::iterator iter;
	for (iter = rooms.begin(); iter != rooms.end(); iter++){
		if ((*iter)->roomNumber == roomNumber){
			return (*iter);
		}
	}

	return NULL;
}

int CRoomManager::enterRoom(CPlayer* p, int roomNumber){
	CRoom* room = findRoom(roomNumber);
	if (room == NULL)
		return fail_signal::fs_no_exist;
	if (room->players.size() > PLAYER_MAX)
		return fail_signal::fs_overflow;

	// nickname check
	std::list<CRoom*>::iterator iter = roomManager.rooms.begin();
	for (; iter != roomManager.rooms.end(); ++iter)
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

	room->playerEnter(p);

	printInfo();

	return -1;
}

int CRoomManager::destroyRoom(int roomNumber)
{
	CRoom* room = findRoom(roomNumber);
	if (room == NULL)
		return fail_signal::fs_no_exist;

	// Sending the kick message to all players in the room to be destroyed.
	t_kick tmpKick;

	tmpKick.type = pkt_type::pt_kick;

	room->broadcast_msg((char*)&tmpKick, sizeof(t_kick));

	for (std::list<CPlayer*>::iterator iter = room->players.begin();
		iter != room->players.end(); ++iter)
	{
		(*iter)->roomNum = -1;
	}

	rooms.remove(room);

	printInfo();

	return -1;
}

bool CRoomManager::leaveRoom(CPlayer* p, int roomNumber)
{
	CRoom* room = findRoom(roomNumber);
	if (room == NULL){
		printf("No ROOM!\n");
		return false;
	}
	room->playerQuit(p, true);

	printInfo();

	return true;
}