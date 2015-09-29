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

	position = 0;
	remainBytes = 0;

	this->peer = peer;
}

CUserToken::~CUserToken()
{
}

bool CUserToken::operator==(const CUserToken& right)
{
	return this->clientSocket == right.clientSocket;
}

bool CUserToken::recieveProcess()
{
	if (this->peer != NULL){
		return this->peer->recieveProcess();
	}
	else
	{
		std::cout << "There is a weired CUserToken which has no player." << std::endl;

		return false;
	}
}

void CUserToken::remove()
{
	if (this->peer != NULL){
		this->peer->remove();

		delete this->peer;
	}
}

bool CUserToken::read_until()
{
	int read_size = recv(clientSocket, buf + position, remainBytes, 0);
	//printf("%d\n", read_size);
	if (read_size == SOCKET_ERROR){
		/* client recieve error! */;
		return false;
	}
	else if (read_size == 0){
		return false;
	}

	position += read_size;
	remainBytes -= read_size;
	return true;
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