#pragma once
#include "433Net.h"

class Player : public IPeer{
	
public:
	std::string nickname;
	int roomNum;
	UserToken token;

	Player();

	void send_msg(char *buf, int size);

	void recieve_msg(char* buf, int size);

	void remove();
};