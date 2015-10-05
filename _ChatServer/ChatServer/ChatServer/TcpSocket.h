#pragma once

#include "stdafx.h"

#define BUFSIZE 8192


class TcpSocket
{

public:
	TcpSocket();

public:
	void Init();
	void InitBuf();
	void InitAct(Proactor* proactor, Acceptor* acceptor, Disconnector* disconnector,
		Connector* connector, Sender* sender, Receiver* receiver);

	SOCKET GetSocket() const;

public:
	void Recv();
	void Recv(char* buf, int buflen);
	void Send(char* buf, int buflen);
	void Reuse();
	void Disconnect();
	void Connect();

	virtual void RecvProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void SendProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void AcceptProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;
	virtual void ConnProcess(bool isError, Act* act, DWORD bytes_transferred) = 0;

public:
	enum ACT_TYPE
	{
		ACT_SEND = 0,
		ACT_RECV,
		ACT_ACCEPT,
		ACT_DISCONNECT,
		ACT_CONNECT,
		ACT_TYPE_CNT
	};

	TcpAct			Act_[ACT_TYPE_CNT];

public:
	SOCKET			Socket_;
	SOCKADDR_IN		Addr_;

public:
	char			AcceptBuf_[BUFSIZE]; //may not use

	char			RecvBuf_[BUFSIZE];
	char			SendBuf_[BUFSIZE]; //may not use

	WSABUF			wsaRecvBuf;
	WSABUF			wsaSendBuf;

public:
	Proactor*		Proactor_;
	Acceptor*		Acceptor_;
	Disconnector*	Disconnector_;
	Sender*			Sender_;
	Receiver*		Receiver_;
	Connector* Connector_;

};
