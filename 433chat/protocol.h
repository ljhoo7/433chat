////////////////////////////////////////
// ������ ������ Ŭ���̾�Ʈ �� �����͸� �ְ� ���� �� ����ϴ� ��Ŷ�� ������ ����Դϴ�.

#pragma once

#define PKTLEN 512
#define STRSIZE	   512

enum pkt_type { pt_any, pt_join, pt_chat, pt_interserver };

typedef struct{
	short length;
	short type;
	char payload[PKTLEN - 4];
}t_any;

//typedef struct{
//	short length;
//	short type;
//
//}t_create;
//
//typedef struct{
//	short length;
//	short type;
//
//}t_destroy;

typedef struct{
	short length;
	short type;

	int room_num;
}t_join;

//typedef struct{
//	short length;
//	short type;
//
//}t_leave;

typedef struct{
	short length;
	short type;

	int room_num;
	// �ִ� 511 �� ���� (���� ���� ���� 512�� )
	char str[STRSIZE];
}t_chat;

typedef union{
	t_any			m_any;
	//t_create	m_create;
	//t_destroy	m_destroy;
	t_join			m_join;
	//t_leave	m_leave;
	t_chat			m_chat;
}t_packet;