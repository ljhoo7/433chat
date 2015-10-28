#include "stdafx.h"

CRoom::CRoom(int roomNumber)
{
	this->roomNumber = roomNumber;

	InitializeCriticalSectionAndSpinCount(&playerLock, 4000);
}
CRoom::~CRoom()
{
	std::list<CPlayer*>::iterator iter;
	for (iter = players.begin(); iter != players.end(); iter++)
	{
		PlayerQuit(*iter, false);
	}

	DeleteCriticalSection(&playerLock);
}

void CRoom::PlayerEnter(CPlayer* player)
{
	player->roomNum = roomNumber;

	t_join_alarm		tmpJoinAlarm;
	memcpy(tmpJoinAlarm.nickname, player->nickname.c_str(), 20);
	tmpJoinAlarm.room_num = roomNumber;
	tmpJoinAlarm.type = pkt_type::pt_join_alarm;

	this->BroadcastMsg((char*)&tmpJoinAlarm, sizeof(t_join_alarm));
	PRINT("[Room] join alarm message has been sent.\n");

	/* informing the entered */
	EnterCriticalSection(&playerLock);
	players.push_back(player);
	LeaveCriticalSection(&playerLock);
	
}


void CRoom::PlayerQuit(CPlayer* player, bool msg)
{
	if (player->roomNum != this->roomNumber) return;


	EnterCriticalSection(&playerLock);
	
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
	if (!check){
		LeaveCriticalSection(&playerLock);
		return;
	}
	t_leave_alarm		tmpLeaveAlarm;
	memcpy(tmpLeaveAlarm.nickname, player->nickname.c_str(), sizeof(tmpLeaveAlarm.nickname));
	tmpLeaveAlarm.room_num = this->roomNumber;
	tmpLeaveAlarm.type = pkt_type::pt_leave_alarm;


	player->roomNum = -1;
	player->nickname = "";
	players.remove(player);

	LeaveCriticalSection(&playerLock);

	/* informing the exited */
	if (msg) this->BroadcastMsg((char*)&tmpLeaveAlarm, sizeof(t_leave_alarm));
	PRINT("[Room] leave alarm message has been sent.\n");
}

void CRoom::BroadcastMsg(char* msg, int size)
{
	EnterCriticalSection(&playerLock);
	if (players.size() == 0){
		LeaveCriticalSection(&playerLock);
		return;
	}

	std::list<CPlayer*>::iterator iter;
	//PRINT("[Room] CRoom %d : %d persons are connecting...\n", roomNumber, players.size());
	for (iter = players.begin(); iter != players.end(); iter++)
	{
		if ((*iter)->serverNum == chatServer->serverNum) (*iter)->Send(msg, size);
	}
	LeaveCriticalSection(&playerLock);
}

int CRoom::GetPlayerSize(){
	int ret = 0;
	EnterCriticalSection(&playerLock);
	ret = players.size();
	LeaveCriticalSection(&playerLock);
	return ret;
}

bool CRoom::NickNameCheck(const char* nick){
	EnterCriticalSection(&playerLock);
	std::list<CPlayer*>::iterator iter2 = players.begin();
	for (; iter2 != players.end(); ++iter2)
	{
		if (!strcmp((*iter2)->nickname.c_str(), nick)){
			LeaveCriticalSection(&playerLock);
			return false;
		}
	}
	LeaveCriticalSection(&playerLock);

	return true;
}

void CRoom::KickAllPlayer(){
	EnterCriticalSection(&playerLock);
	t_kick tmpKick;

	tmpKick.type = pkt_type::pt_kick;

	BroadcastMsg((char*)&tmpKick, sizeof(t_kick));

	std::list<CPlayer*>::iterator iter;

	for (iter = players.begin(); iter != players.end(); iter++)
	{
		if (chatServer->agentServer->socket->isConnected)
			chatServer->agentServer->socket->UserInfoSend(false, *iter, 3);
	}
	for (std::list<CPlayer*>::iterator iter = players.begin();
		iter != players.end(); ++iter)
	{
		(*iter)->roomNum = -1;
	}
	LeaveCriticalSection(&playerLock);
}
