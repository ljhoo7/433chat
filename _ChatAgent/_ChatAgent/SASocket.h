#pragma once

#include "stdafx.h"

class SASocket : public TcpSocket
{
private:
	struct mswsock_s {
		LPFN_CONNECTEX ConnectEx;
	} mswsock;

	BOOL LoadMswsock(void);

public:
	SASocket(ServerAgent* _serverAgent);
	~SASocket();

public:
	ServerAgent* serverAgent;

public:
	void RecvProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void SendProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, CAct* act, DWORD bytes_transferred);

	void Bind(bool reuse);
	void Connect(DWORD ip, WORD port);
};
