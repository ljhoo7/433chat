#pragma once

#include "stdafx.h"

class InterSocket : public TcpSocket
{
public:
	InterSocket(TcpInterServer* InterServer);
	~InterSocket();

public:
	TcpInterServer* interServer_;

public:
	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);
};
