
#include "stdafx.h"

TcpSocket::TcpSocket()
{
	socket_ = INVALID_SOCKET;
	acceptor_ = NULL;
}

void TcpSocket::Init()
{
	socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (socket_ == INVALID_SOCKET)
	{
		PRINTF(L"WSASocket() Error!!! err(%d)\n", WSAGetLastError());
	}

	InitBuf();
}

void TcpSocket::InitBuf()
{
	wsaRecvBuf.buf = recvBuf_;
	wsaRecvBuf.len = BUFSIZE;

	wsaSendBuf.buf = sendBuf_;
	wsaSendBuf.len = BUFSIZE;

	ZeroMemory(recvBuf_, BUFSIZE);
	ZeroMemory(sendBuf_, BUFSIZE);

	ZeroMemory(acceptBuf_, BUFSIZE);
}

void TcpSocket::InitAct(Proactor* proactor, Acceptor* acceptor, Disconnector* disconnector,
	Connector* connector, Sender* sender, Receiver* receiver)
{
	proactor_ = proactor;
	acceptor_ = acceptor;
	disconnector_ = disconnector;
	sender_ = sender;
	receiver_ = receiver;
	connector_ = connector;

	act_[ACT_ACCEPT].Init(acceptor_, this);
	act_[ACT_RECV].Init(receiver_, this);
	act_[ACT_SEND].Init(sender_, this);
	act_[ACT_DISCONNECT].Init(disconnector_, this);
	act_[ACT_CONNECT].Init(connector_, this);
}

void TcpSocket::Recv()
{
	DWORD recvbytes = 0;
	DWORD flags = 0;


	INT ret = WSARecv(socket_, &(wsaRecvBuf), 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(&(act_[ACT_RECV])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			PRINTF(L"WSARecv() Error!!! s(%d) err(%d)\n", socket_, error);
			Disconnect();
		}
	}

	PRINTF(L"%d bytes were received ! - Recv()\n", recvbytes);
}

void TcpSocket::Recv(char* buf, int buflen)
{
	DWORD recvbytes = 0;
	DWORD flags = 0;
	wsaRecvBuf.buf = buf;
	wsaRecvBuf.len = buflen;

	INT ret = WSARecv(socket_, &(wsaRecvBuf), 1, &recvbytes, &flags, static_cast<OVERLAPPED*>(&(act_[ACT_RECV])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			PRINTF(L"WSARecv() Error!!! s(%d) err(%d)\n", socket_, error);
			Disconnect();
		}
	}
	PRINTF(L"%d bytes were received !\n", recvbytes);
}

void TcpSocket::Send(char* buf, int buflen)
{
	if (buflen == 0) return;
	DWORD sentbytes = 0;
	wsaSendBuf.buf = buf;
	wsaSendBuf.len = buflen;

	INT ret = WSASend(socket_, &(wsaSendBuf), 1, &sentbytes, 0, static_cast<OVERLAPPED*>(&(act_[ACT_SEND])), NULL);

	if (ret != 0)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			PRINTF(L"WSASend() Error!!! s(%d) err(%d)\n", socket_, error);
			//Disconnect();
		}
	}

	PRINTF(L"%d bytes were sent !\n", sentbytes);
}

void TcpSocket::Reuse()
{
	acceptor_->Register(*this);
}

void TcpSocket::Disconnect()
{
	BOOL ret = TransmitFile(
		socket_,
		NULL,
		0,
		0,
		static_cast<OVERLAPPED*>(&(act_[ACT_DISCONNECT])),
		NULL,
		TF_DISCONNECT | TF_REUSE_SOCKET
		);

	if (!ret)
	{
		int error = WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			PRINTF(L"already disconnected, DisconnectEx Error!!! s(%d), err(%d)\n", socket_, error);
		}
	}
}