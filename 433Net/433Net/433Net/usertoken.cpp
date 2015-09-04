#include "usertoken.h"

UserToken::UserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr, IPeer* peer)
:clientSocket(NULL)
{
	this->clientSocket = clientSocket;
	this->clientAddr = clientAddr;

	position = 0;
	remainBytes = 0;

	this->peer = peer;
}

UserToken::~UserToken(){

}

bool UserToken::operator==(const UserToken& right){
	return this->clientSocket == right.clientSocket;
}

bool UserToken::recieveProcess(){
	if (this->peer != NULL){
		return this->peer->recieveProcess();
	}
	else
	{
		std::cout << "There is a weired UserToken which has no player." << std::endl;
		
		return false;
	}
}

void UserToken::remove(){
	if (this->peer != NULL){
		this->peer->remove();

		delete this->peer;
	}
}

bool UserToken::read_until(){
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

void UserToken::on_msg(char* buf, int size){

	if (peer != NULL){
		peer->recieve_msg(buf, size);
	}
}

void UserToken::send_msg(char *buf, int len){
	send(clientSocket, buf, len, 0);
}