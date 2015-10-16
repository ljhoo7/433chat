#pragma once

class CPacket
{
public:
	TcpSocket* owner;
	char* msg;

	CPacket();
	CPacket(TcpSocket* owner, char* msg);

	~CPacket();
};

