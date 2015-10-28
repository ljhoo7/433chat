#pragma once

class InterSocket : public TcpSocket
{
public:
	InterSocket(TcpInterServer* InterServer, bool isConnect);
	~InterSocket();

public:
	TcpInterServer* interServer_;
	int serverNum;
	bool isConnect;
	bool isUse;

	MemPooler<msg_buffer> *poolManager;
	MemPooler<CPacket> *packetPoolManager;

public:
	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void packetHandling(CPacket *packet);

	void Bind(bool reuse);
	void Connect(unsigned int ip, WORD port, int serverNum);

	void SendPlayerInfo();
	void SendRoomInfo();
	void MakeSync();

	CPlayer* FindPlayerBySocket(SOCKET socket);

public:
	std::thread heartbeatThread;
	bool beatCheck;
	void HeartbeatCheck();

private:
	struct mswsock_s {
		LPFN_CONNECTEX ConnectEx;
	} mswsock;

	BOOL LoadMswsock(void);

private:
	int position;
	int remainBytes;
	bool isVar;
};
