#include "stdafx.h"

CUserToken::CUserToken(SOCKET clientSocket)
{
	this->clientSocket = clientSocket;
}

CUserToken::CUserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr, IPeer* peer)
	:clientSocket(NULL)
{
	this->clientSocket = clientSocket;
	this->clientAddr = clientAddr;


	SocketContext.token = this;
	SocketContext.recvContext.wsaBuf.buf = SocketContext.recvContext.Buffer;
	SocketContext.recvContext.position = 0;
	SocketContext.recvContext.remainBytes = HEADER_SIZE;

	this->peer = peer;
}


void CUserToken::_recieve(char* buf, int size){
	if (size == 0) return;
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;
	ZeroMemory(&SocketContext.recvContext.overlapped, sizeof(WSAOVERLAPPED));

	SocketContext.recvContext.wsaBuf.buf = buf;
	SocketContext.recvContext.wsaBuf.len = size;
	SocketContext.recvContext.remainBytes = size;
	int ret = WSARecv(SocketContext.token->clientSocket, &(SocketContext.recvContext.wsaBuf), 1,
		&dwRecvBytes, &dwFlags, &(SocketContext.recvContext.overlapped), NULL);

	if (SOCKET_ERROR == ret){
		int ErrCode = WSAGetLastError();
		if (ErrCode != WSA_IO_PENDING){
			printf("interserver recieve error!");
			remove();
		}
	}

	return;
}

CUserToken::~CUserToken()
{
}

bool CUserToken::operator==(const CUserToken& right)
{
	return this->clientSocket == right.clientSocket;
}

void CUserToken::recieveProcess()
{
	if (this->peer != NULL){
		this->peer->recieveProcess();
	}
	else
	{
		std::cout << "There is a weired CUserToken which has no player." << std::endl;
	}
}

void CUserToken::remove()
{
	if (this->peer != NULL){
		this->peer->remove();

		delete this->peer;
	}
}


void CUserToken::on_msg(char* buf, int size)
{
	if (peer != NULL){
		peer->recieve_msg(buf, size);
	}
}

void CUserToken::send_msg(char *buf, int len)
{
	send(clientSocket, buf, len, 0);
}