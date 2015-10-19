#pragma once

class AgentSocket : public TcpSocket
{
public:
	AgentSocket();
	AgentSocket(int serverNum);

	~AgentSocket();

public:
	int serverNum;

	int position;
	int remainBytes;
	bool isVar;

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

	void MakeSync();
	void UserInfoSend();
	void RoomInfoSend();
	void InterServerInfoSend();
};
