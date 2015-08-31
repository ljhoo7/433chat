#pragma once
#include "433Net.h"

class Player : public IPeer{
	
public:
	std::string nickname;
	int roomNum;
	UserToken token;

	Player(std::string nickname, int roomNum, UserToken token){
		this->nickname = nickname;
		this->roomNum = roomNum;
		this->token = token;
	}

	void send_msg(char *buf, int size){
		/* packet handling*/
		this->token.send_msg(buf, size);
	}

	void recieve_msg(char* buf, int size){
		t_packet pkt;
		memcpy(&pkt, buf, size);
		switch (pkt.m_any.type)
		{
		case pkt_type::pt_chat:
			pkt.m_chat.str[size] = '\0';
			printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", size);
			printf("[받은 데이터] %s\n", pkt.m_chat.str);
			break;
		}
	}
};