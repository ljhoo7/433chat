#pragma once

class CRoom
{
public:
	CRoom(int roomNumber);
	~CRoom();

	void PlayerEnter(CPlayer* player);
	void PlayerQuit(CPlayer* player, bool msg);
	void BroadcastMsg(char* msg, int size);

	int roomNumber;
	std::list<CPlayer*> players;
	CRITICAL_SECTION playerLock;

	int GetPlayerSize();
	bool NickNameCheck(const char* nick);
	void KickAllPlayer();
};

