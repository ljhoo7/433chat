#pragma once

#include "stdafx.h"

class SASocket : public TcpSocket
{
public:
	SASocket();
	virtual ~SASocket();

public:
	void PacketHandling(char* buf);
	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);

	void HeartbeatCheck();

public:
	bool		 inUse;
	std::thread	 heartbeatThread;
	bool		 beatCheck;
	
public:

	int			 mServerNum;
	int			 mPosition;
	int			 mRemainBytes;
	bool		 mIsVar;
	
};
