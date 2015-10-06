#pragma once

#include "stdafx.h"

class TcpInterServer : public WinSockBase
{
public:
	TcpInterServer();
	TcpInterServer(WORD Port, int ThreadPoolSize);
	void Start(bool connect);

public:
	Proactor*		proactor_;
	Acceptor*		acceptor_;
	Receiver*		receiver_;
	Sender*			sender_;
	Disconnector*	disconnector_;
	Connector*		connector_;

	TcpListenSocket	listenSocket_;

public:
	TcpSocket* socket;

	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void packetHandling(CPacket *packet);

	void Connect(char* ip, WORD port);

	bool beatCheck;
	int position;
	int remainBytes;
	bool isVar;
	bool isConnect;
	bool isUse;
	MemPooler<msg_buffer> *poolManager;
	MemPooler<CPacket> *packetPoolManager;

public:
	std::thread heartThread;
	void heartbeatCheck();

private:
	WORD			port_;
	int				threadPoolSize_;

	void SendPlayerInfo();
	void SendRoomInfo();
	void MakeSync();
};