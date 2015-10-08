#include "stdafx.h"

CRoom::CRoom(int roomNumber)
{
	this->roomNumber = roomNumber;
}
CRoom::~CRoom()
{
	std::list<CPlayer*>::iterator iter;
	for (iter = players.begin(); iter != players.end(); iter++)
	{
		PlayerQuit(*iter, false);
	}
}

void CRoom::PlayerEnter(CPlayer* player)
{
	player->roomNum = roomNumber;

	t_join_alarm		tmpJoinAlarm;
	memcpy(tmpJoinAlarm.nickname, player->nickname.c_str(), 20);
	tmpJoinAlarm.room_num = roomNumber;
	tmpJoinAlarm.type = pkt_type::pt_join_alarm;

	this->BroadcastMsg((char*)&tmpJoinAlarm, sizeof(t_join_alarm));
	PRINTF(L"join alarm message has been sent.\n");

	/* informing the entered */
	players.push_back(player);
}


void CRoom::PlayerQuit(CPlayer* player, bool msg)
{
	if (player->roomNum != this->roomNumber) return;
	std::list<CPlayer*>::iterator iter;
	bool check = false;
	for (iter = players.begin(); iter != players.end(); iter++)
	{
		if ((*iter) == player)
		{
			check = true;
			break;
		}
	}
	if (!check) return;
	t_leave_alarm		tmpLeaveAlarm;
	memcpy(tmpLeaveAlarm.nickname, player->nickname.c_str(), sizeof(tmpLeaveAlarm.nickname));
	tmpLeaveAlarm.room_num = this->roomNumber;
	tmpLeaveAlarm.type = pkt_type::pt_leave_alarm;


	player->roomNum = -1;
	player->nickname = "";
	players.remove(player);

	/* informing the exited */
	if (msg) this->BroadcastMsg((char*)&tmpLeaveAlarm, sizeof(t_leave_alarm));
	PRINTF(L"leave alarm message has been sent.\n");
}

void CRoom::BroadcastMsg(char* msg, int size)
{
	std::list<CPlayer*>::iterator iter;
	//PRINTF(L"CRoom %d : %d persons are connecting...\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++)
	{
		if ((*iter)->isMine) (*iter)->Send(msg, size);
	}
}