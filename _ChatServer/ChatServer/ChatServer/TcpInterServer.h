#pragma once

#include "stdafx.h"

class TcpInterServer : public WinSockBase
{
public:
	TcpInterServer();
	TcpInterServer(WORD Port, int ThreadPoolSize);
	void Start(bool connect);

public:
	Proactor*		Proactor_;
	Acceptor*		Acceptor_;
	Receiver*		Receiver_;
	Sender*			Sender_;
	Disconnector*	Disconnector_;
	Connector*		Connector_;

	TcpListenSocket	ListenSocket_;

public:
	TcpSocket* socket;

	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void packetHandling(CPacket *packet);

	void Connect(char* ip, WORD port);

	bool beat_check;
	int position;
	int remainBytes;
	bool isVar;
	bool isConnect;
	bool isUse;
	MemPooler<msg_buffer> *poolManager;
	MemPooler<CPacket> *packetPoolManager;


private:
	WORD			Port_;
	int				ThreadPoolSize_;

	void SendPlayerInfo();
	void SendRoomInfo();
	void MakeSync();
};