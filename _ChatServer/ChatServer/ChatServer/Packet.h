#pragma once

class CPacket
{
public:
	// 2 : client, 1 : listen server, 0: connect server
	int type;
	TcpSocket* owner;
	char* msg;

	CPacket();
	CPacket(TcpSocket* owner, char* msg, int type);

	~CPacket();
};

