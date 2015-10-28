#pragma once

class AgentSocket : public TcpSocket
{
public:
	AgentSocket();
	AgentSocket(int serverNum);

	~AgentSocket();

public:
	int serverNum;
	bool isConnected;

	MemPooler<msg_buffer> *poolManager;
	MemPooler<CPacket> *packetPoolManager;

public:
	bool ValidPacket(CPacket *packet);
	void PacketHandling(CPacket *packet);

	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);

	void RemoveAgent();

	void Bind(bool reuse);
	void Connect(unsigned int ip, WORD port);

	void MakeSync();
	void UserInfoSend(bool isTotal, CPlayer* player, char connect);
	void RoomInfoSend(bool isTotal, int roomNum, bool create);
	//void InterServerInfoSend(bool isTotal, int serverNum, bool connect);


private:
	struct mswsock_s 
	{
		LPFN_CONNECTEX ConnectEx;
	} mswsock;

	BOOL LoadMswsock(void);

private:
	int position;
	int remainBytes;
	bool isVar;

};
