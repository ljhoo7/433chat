#pragma once

#include "stdafx.h"

class TcpSocket
{

public:
	TcpSocket();
	virtual ~TcpSocket();

public:
	void Init();
	void InitBuf();
	void InitAct(Proactor* proactor, Acceptor* acceptor, Disconnector* disconnector,
		Connector* connector, CSender* sender, CReceiver* receiver);

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

	TcpAct			act_[ACT_TYPE_CNT];

public:
	SOCKET			socket_;
	SOCKADDR_IN		addr_;

public:
	char			acceptBuf_[BUFSIZE]; //may not use

	char			recvBuf_[BUFSIZE];
	char			sendBuf_[BUFSIZE]; //may not use

	WSABUF			wsaRecvBuf;
	WSABUF			wsaSendBuf;

public:
	Proactor*		proactor_;
	Disconnector*	disconnector_;
	Acceptor*		acceptor_;
	CSender*		sender_;
	CReceiver*		receiver_;
	Connector*		connector_;

};
