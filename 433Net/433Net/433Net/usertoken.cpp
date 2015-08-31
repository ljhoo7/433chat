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
			printf("remain : %d\n", remainBytes);
		}
		bool check =  read_until();
		if (remainBytes <= 0){
			position = 0;
			printf("remain : %d\n", remainBytes);
			on_msg(buf, size);
		}
		return check;
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
	/* packet handling*/
/*	t_packet pkt;
	memcpy(&pkt, buf, size);
	switch (pkt.m_any.type)
	{
		case pkt_type::pt_chat:
			pkt.m_chat.str[size] = '\0';
			printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", size);
			printf("[받은 데이터] %s\n", pkt.m_chat.str);
			break;
	}*/
	if (peer != NULL){
		peer->recieve_msg(buf, size);
	}
}

void UserToken::send_msg(char *buf, int len){
	send(clientSocket, buf, len, 0);
}