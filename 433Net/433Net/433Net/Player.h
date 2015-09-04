#pragma once
#include "utilities.h"
#include "logic.h"
#include "usertoken.h"
#include "bufPoolManager.h"

class Player : public IPeer{

public:
	bool isMine;
	std::string nickname;
	int roomNum;
	UserToken token;

	int identifier;				// token ( it has the other meaning )

	BufPoolManager poolManager;
	PacketPoolManager packetPoolManager;

	Player(bool isMine);

	bool recieve(char* buf, int size);
	bool recieveProcess();
	void send_msg(char *buf, int size);
	void recieve_msg(char* buf, int size);
	void remove();
	void disconnect();

	void packetHandling(Packet *packet);
};