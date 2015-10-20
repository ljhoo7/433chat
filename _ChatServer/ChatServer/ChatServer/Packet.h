#pragma once

class CPacket
{
public:
	TcpSocket* owner;
	char* msg;
	bool isAgent;

	CPacket();
	CPacket(TcpSocket* owner, char* msg);

	~CPacket();
};

