////////////////////////////////////////
// ������ ������ Ŭ���̾�Ʈ �� �����͸� �ְ� ���� �� ����ϴ� ��Ŷ�� ������ ����Դϴ�.

#pragma once

#define PKTLEN 512
#define STRSIZE	   508

enum pkt_type { pt_any, pt_create, pt_destroy, pt_join, pt_leave, pt_chat };

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

	// �ִ� 507 �� ���� (���� ���� ���� 508�� )
	char str[STRSIZE];
}t_chat;

typedef union{
	t_any		m_any;
	t_create	m_create;
	t_destroy	m_destroy;
	t_join		m_join;
	t_leave		m_leave;
	t_chat		m_chat;
}t_packet;