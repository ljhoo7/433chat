#pragma once

#define BUFSIZE 8192


class TcpSocket
{

public:
	TcpSocket();

public:
	void Init();
	void InitBuf();
	void InitAct(Proactor* proactor, Acceptor* acceptor, Disconnector* disconnector, Sender* sender, Receiver* receiver);

	SOCKET GetSocket() const;

public:
	void Recv();
	void Recv(BYTE* buf, int buflen);
	void Send(BYTE* buf, int buflen);
	void Reuse();
	void Disconnect();

public:
	enum ACT_TYPE
	{
		ACT_SEND = 0,
		ACT_RECV,
		ACT_ACCEPT,
		ACT_DISCONNECT,
		ACT_TYPE_CNT
	};

	TcpAct			Act_[ACT_TYPE_CNT];

public:
	SOCKET			Socket_;
	SOCKADDR_IN		Addr_;

public:
	char			AcceptBuf_[BUFSIZE];

	char			RecvBuf_[BUFSIZE];
	char			SendBuf_[BUFSIZE];

	WSABUF			wsaRecvBuf;
	WSABUF			wsaSendBuf;

public:
	Proactor*		Proactor_;
	Acceptor*		Acceptor_;
	Disconnector*	Disconnector_;
	Sender*			Sender_;
	Receiver*		Receiver_;

};
