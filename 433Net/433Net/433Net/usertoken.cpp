#include "usertoken.h"

UserToken::UserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr, IPeer* peer){
	this->clientSocket = clientSocket;
	this->clientAddr = clientAddr;

	position = 0;
	remainBytes = 0;
	size = 0;

	this->peer = peer;
}

UserToken::~UserToken(){

}

bool UserToken::operator==(const UserToken& right){
	return this->clientSocket == right.clientSocket;
}

bool UserToken::recieveProcess(){
	
	if (position < HEADER_SIZE){
		if (position == 0){
			remainBytes = HEADER_SIZE;
		}
		
		return read_until();
	}
	else{
		if (position == HEADER_SIZE){
			
			memcpy(&remainBytes, buf, sizeof(short));
			size = (int)(remainBytes);
			remainBytes -= HEADER_SIZE;
		}
		bool check =  read_until();
		if (remainBytes <= 0){
			position = 0;
			on_msg(buf, size);
		}
		return check;
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