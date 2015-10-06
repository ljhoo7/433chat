
#include "stdafx.h"

TcpSocket::TcpSocket()
{
	Socket_ = INVALID_SOCKET;
	Acceptor_ = NULL;
}

void TcpSocket::Init()
{
	Socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (Socket_ == INVALID_SOCKET)
	{
		printf("WSASocket() Error!!! err(%d)\n", WSAGetLastError());
	}

	InitBuf();
}

void TcpSocket::InitBuf()
{
	wsaRecvBuf.buf = RecvBuf_;
	wsaRecvBuf.len = BUFSIZE;

	wsaSendBuf.buf = SendBuf_;
	wsaSendBuf.len = BUFSIZE;

	ZeroMemory(RecvBuf_, BUFSIZE);
	ZeroMemory(SendBuf_, BUFSIZE);

	ZeroMemory(AcceptBuf_, BUFSIZE);
}

void TcpSocket::InitAct(Proactor* proactor, Acceptor* acceptor, Disconnector* disconnector,
	Connector* connector, Sender* sender, Receiver* receiver)
{
	Proactor_ = proactor;
	Acceptor_ = acceptor;
	Disconnector_ = disconnector;
	Sender_ = sender;
	Receiver_ = receiver;
	Connector_ = connector;

	Act_[ACT_ACCEPT].Init(Acceptor_, this);
	Act_[ACT_RECV].Init(Receiver_, this);
	Act_[ACT_SEND].Init(Sender_, this);
	Act_[ACT_DISCONNECT].Init(Disconnector_, this);
	Act_[ACT_CONNECT].Init(Connector_, this);
}

void TcpSocket::Recv()
{
	DWORD recvbytes = 0;
	DWORD flags = 0;


	INT ret = WSARecv(Socket_, &(wsaRecvBuf), 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(&(Act_[ACT_RECV])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("WSARecv() Error!!! s(%d) err(%d)\n", Socket_, error);
			Disconnect();
		}
	}
}

void TcpSocket::Recv(char* buf, int buflen)
{
	DWORD recvbytes = 0;
	DWORD flags = 0;
	wsaRecvBuf.buf = buf;
	wsaRecvBuf.len = buflen;

	INT ret = WSARecv(Socket_, &(wsaRecvBuf), 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(&(Act_[ACT_RECV])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("WSARecv() Error!!! s(%d) err(%d)\n", Socket_, error);
			Disconnect();
		}
	}
}

void TcpSocket::Send(char* buf, int buflen)
{
	if (buflen == 0) return;
	DWORD sentbytes = 0;
	wsaSendBuf.buf = buf;
	wsaSendBuf.len = buflen;

	INT ret = WSASend(Socket_, &(wsaSendBuf), 1, &sentbytes, 0, static_cast<OVERLAPPED*>(&(Act_[ACT_SEND])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("WSASend() Error!!! s(%d) err(%d)\n", Socket_, error);
			//Disconnect();
		}
	}
}

void TcpSocket::Reuse()
{
	Acceptor_->Register(*this);
}

void TcpSocket::Disconnect()
{
	BOOL ret = TransmitFile(
		Socket_,
		NULL,
		0,
		0,
		static_cast<OVERLAPPED*>(&(Act_[ACT_DISCONNECT])),
		NULL,
		TF_DISCONNECT | TF_REUSE_SOCKET
		);

	if (!ret)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			printf("DisconnectEx Error!!! s(%d), err(%d)\n", Socket_, error);
		}
	}
}