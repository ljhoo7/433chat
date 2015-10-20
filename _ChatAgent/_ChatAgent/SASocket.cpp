#include "stdafx.h"

BOOL SASocket::LoadMswsock(void){
	SOCKET sock;
	DWORD dwBytes;
	int rc;

	/* Dummy socket needed for WSAIoctl */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return FALSE;

	{
		GUID guid = WSAID_CONNECTEX;
		rc = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guid, sizeof(guid),
			&mswsock.ConnectEx, sizeof(mswsock.ConnectEx),
			&dwBytes, NULL, NULL);
		if (rc != 0)
			return FALSE;
	}

	rc = closesocket(sock);
	if (rc != 0)
		return FALSE;

	return TRUE;
}
SASocket::SASocket(ServerAgent* _serverAgent){
	serverAgent = _serverAgent;

	if (!LoadMswsock()) {
		PRINTF("Error loading mswsock functions: %d\n", WSAGetLastError());
		return;
	}
}

void SASocket::Bind(bool reuse){
	if (!reuse){
		socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (socket_ == INVALID_SOCKET)
		{
			PRINTF("WSASocket() Error!!! err(%d)\n", WSAGetLastError());
		}

	}
	int rc;
	struct sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;
	rc = bind(socket_, (SOCKADDR*)&addr, sizeof(addr));
	if (rc != 0) {
		PRINTF("bind failed: %d\n", WSAGetLastError());
		return;
	}
}

void SASocket::RecvProcess(bool isError, CAct* act, DWORD bytes_transferred){
	this->serverAgent->RecvProcess(isError, act, bytes_transferred);
}

void SASocket::SendProcess(bool isError, CAct* act, DWORD bytes_transferred){
	this->serverAgent->SendProcess(isError, act, bytes_transferred);
}


void SASocket::DisconnProcess(bool isError, CAct* act, DWORD bytes_transferred){
	this->serverAgent->DisconnProcess(isError, act, bytes_transferred);
}

void SASocket::ConnProcess(bool isError, CAct* act, DWORD bytes_transferred){
	this->serverAgent->ConnProcess(isError, act, bytes_transferred);
}

void SASocket::Connect(DWORD ip, WORD port){
	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip; // google.com
	addr.sin_port = htons(port);

	int ok = mswsock.ConnectEx(socket_, (SOCKADDR*)&addr, sizeof(addr), NULL, 0, NULL,
		static_cast<OVERLAPPED*>(&act_[TcpSocket::ACT_CONNECT]));
	if (ok) {


		PRINTF("ConnectEx succeeded immediately\n");
	}

	int error = WSAGetLastError();
	if (ok == FALSE && WSAGetLastError() != ERROR_IO_PENDING) {
		PRINTF("ConnectEx Error!!! s(%d), err(%d)\n", socket_, error);
	}
	PRINTF("Connect Request..\n");
}