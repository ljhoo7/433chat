#pragma once

#include "stdafx.h"

class InterConnectSocket : public TcpSocket
{
private:
	struct mswsock_s {
		LPFN_CONNECTEX ConnectEx;
	} mswsock;

	BOOL LoadMswsock(void);

public:
	InterConnectSocket(TcpInterServer* InterServer);
	~InterConnectSocket();

public:
	TcpInterServer* interServer_;

public:
	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);

	void Bind();
	void Connect(char* ip, WORD port);
};
