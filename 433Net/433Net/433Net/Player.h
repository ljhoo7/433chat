#pragma once
#include "433Net.h"

class Player : public IPeer{
	
public:
	std::string nickname;
	int roomNum;
	UserToken token;

	Player(){
		roomNum = -1;
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
			printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", size);
			printf("[���� ������] %s\n", pkt.m_chat.str);
			break;
		}
	}
};