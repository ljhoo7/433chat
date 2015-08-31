#include "usertoken.h"

UserToken::UserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr){
	this->clientSoket = clientSocket;
	this->clientAddr = clientAddr;

	position = 0;
	remainBytes = 0;
}

UserToken::~UserToken(){

}

bool UserToken::operator==(const UserToken& right){
	return this->clientSoket == right.clientSoket;
}

void UserToken::recieveProcess(){
	if (position < HEADER_SIZE){
		if (position == 0){
			remainBytes = HEADER_SIZE;
		}
		read_until();
	}
	else{
		if (position == HEADER_SIZE){
			memcpy(&remainBytes, buf, sizeof(short));
		}
		read_until();
		if (remainBytes <= 0){
			on_msg(buf);
		}
	}
}


void UserToken::read_until(){
	int read_size = recv(clientSoket, buf + position, remainBytes, 0);

	if (read_size == SOCKET_ERROR){
		/* client recieve error! */;
	}

	position += read_size;
	remainBytes -= read_size;
}

void UserToken::on_msg(char* buf){

}