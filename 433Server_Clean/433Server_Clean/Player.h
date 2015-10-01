#pragma once

class CPlayer : public IPeer
{
public:
	CPlayer();
	~CPlayer();

	bool isMine;
	std::string nickname;
	int roomNum;
	CUserToken* token;

	// token ( it has the other meaning )
	int identifier;

	CBufPoolManager poolManager;
	CPacketPoolManager packetPoolManager;

	CPlayer(bool isMine);

	bool valid_Packet(CPacket *packet);
	void playerSync(char *buf, int size);

	void recieve(char* buf, int size);
	void recieveProcess();
	void send_msg(char *buf, int size);
	void recieve_msg(char* buf, int size);
	void remove();
	void disconnect();

	void packetHandling(CPacket *packet);
};

