////////////////////////////////////////
// 다음은 서버와 클라이언트 간 데이터를 주고 받을 때 사용하는 패킷을 정의한 헤더입니다.
#pragma once

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PKTLEN 512

enum pkt_type { pt_any, pt_create, pt_destroy, pt_join, pt_leave, pt_chat };

typedef struct{
	float x, y, z;
}USER_COORD;

typedef struct{
	short length;
	short type;
	char payload[PKTLEN - 4];
}t_any;

typedef struct{
	short length;
	short type;

}t_create;

typedef struct{
	short length;
	short type;

}t_destroy;

typedef struct{
	short length;
	short type;

}t_join;

typedef struct{
	short length;
	short type;

}t_leave;

typedef struct{
	short length;
	short type;



}t_chat;

typedef union{
	t_any		m_any;
	t_create	m_create;
	t_destroy	m_destroy;
	t_join		m_join;
	t_leave		m_leave;
	t_chat		m_chat;
}t_packet;