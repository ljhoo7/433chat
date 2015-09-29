#pragma once

class CPacket
{
public:
	// 2 : client, 1 : listen server, 0: connect server
	int type;
	CUserToken* owner;
	char* msg;

	CPacket();
	CPacket(CUserToken* owner, char* msg, int type);

	~CPacket();
};

