#include "stdafx.h"

BOOL InterConnectSocket::load_mswsock(void){
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
InterConnectSocket::InterConnectSocket(TcpInterServer* InterServer){
	InterServer_ = InterServer;

	if (!load_mswsock()) {
		printf("Error loading mswsock functions: %d\n", WSAGetLastError());
		return;
	}

	int rc;
	struct sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;
	rc = bind(Socket_, (SOCKADDR*)&addr, sizeof(addr));
	if (rc != 0) {
		printf("bind failed: %d\n", WSAGetLastError());
		return;
	}

}

void InterConnectSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->RecvProcess(isError, act, bytes_transferred);
}

void InterConnectSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->SendProcess(isError, act, bytes_transferred);
}

void InterConnectSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	
}

void InterConnectSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->AcceptProcess(isError, act, bytes_transferred);
}

void InterConnectSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->ConnProcess(isError, act, bytes_transferred);
}


void InterConnectSocket::Connect(char* ip, WORD port){
	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip); // google.com
	addr.sin_port = htons(port);

	int ok = mswsock.ConnectEx(Socket_, (SOCKADDR*)&addr, sizeof(addr), NULL, 0, NULL, 
		static_cast<OVERLAPPED*>(&Act_[TcpSocket::ACT_CONNECT]));
	if (ok) {
		printf("ConnectEx succeeded immediately\n");
	}

	int error = WSAGetLastError();
	if (ok == FALSE && WSAGetLastError() == ERROR_IO_PENDING) {
		printf("AcceptEx Error!!! s(%d), err(%d)\n", Socket_, error);
	}
}