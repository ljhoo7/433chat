#pragma once
#include "utilities.h"

class Player : public IPeer{

public:
	std::string nickname;
	int roomNum;
	UserToken token;

	Player();

	bool recieveProcess();
	void send_msg(char *buf, int size);
	void recieve_msg(char* buf, int size);
	void remove();
};