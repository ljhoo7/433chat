#pragma once

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	bool isMine;
	std::string nickname;
	int roomNum;
	TcpSocket* token;
	// token ( it has the other meaning )
	int identifier;

	CPlayer(bool isMine, int cnt);

	bool valid_Packet(CPacket *packet);
	void playerSync(char *buf, int size);
	void remove();
	void packetHandling(CPacket *packet);
};

