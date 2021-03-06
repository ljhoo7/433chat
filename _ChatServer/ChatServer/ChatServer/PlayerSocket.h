#pragma once

struct msg_buffer{
	char buf[BUFSIZE];
};

class CPlayer : public TcpSocket 
{
public:
	CPlayer();
	CPlayer(int serverNum);

	~CPlayer();

public:
	int serverNum;
	std::string nickname;
	int roomNum;
	// token ( it has the other meaning )
	int identifier;

	MemPooler<msg_buffer> *poolManager;
	MemPooler<CPacket> *packetPoolManager;

	std::vector<int> escapingList;

	bool beatCheck;
	
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
	void EscapePlayer(bool first, int i);

private:
	int position;
	int remainBytes;
	bool isVar;
};

