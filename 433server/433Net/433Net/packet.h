#pragma once
#include "utilities.h"

class Packet{
public:
	int type; // 2 : client, 1 : listen server, 0: connect server
	UserToken* owner;
	char* msg;

	Packet();
	Packet(UserToken* owner, char* msg, int type);
};