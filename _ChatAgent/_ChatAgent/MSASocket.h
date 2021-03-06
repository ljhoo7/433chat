#pragma once

#include "stdafx.h"

class MSASocket : public TcpSocket
{
private:
	struct mswsock_s {
		LPFN_CONNECTEX ConnectEx;
	} mswsock;

	BOOL LoadMswsock(void);

public:
	MSASocket(MServerAgent* _serverAgent);
	~MSASocket(){};

public:
	MServerAgent* serverAgent;

public:
	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){}
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);

	void Bind(bool reuse);
	void Connect(DWORD ip, WORD port);
};

