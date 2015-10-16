#pragma once

struct msg_buffer{
	char buf[BUFSIZE];
};

class CPlayer : public TcpSocket 
{
public:
	CPlayer();
	CPlayer(bool isMine);

	~CPlayer();

public:
	bool isMine;
	std::string nickname;
	int roomNum;
	// token ( it has the other meaning )
	int identifier;

	int position;
	int remainBytes;
	bool isVar;

	MemPooler<msg_buffer> *poolManager;
	MemPooler<CPacket> *packetPoolManager;

	
public:
	bool ValidPacket(CPacket *packet);
	void PlayerSync(char *buf, int size);
	void PacketHandling(CPacket *packet);

	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);

	void RemovePlayer();
};

