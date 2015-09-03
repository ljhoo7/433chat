#pragma once

#include<chrono>

#define PKTLEN 512
#define STRSIZE	512

enum pkt_type {
	pt_any,
	pt_create,
	pt_destroy,
	pt_join,
	pt_leave,
	pt_chat,
	pt_interserver,
};

typedef struct{
	unsigned short length;
	unsigned short type;

	int room_num;
}t_create;

typedef struct{
	unsigned short length;
	unsigned short type;

	int room_num;
}t_destroy;

typedef struct{
	unsigned short length;
	unsigned short type;

	int room_num;
	char nickname[STRSIZE];
}t_join;

typedef struct{
	unsigned short length;
	unsigned short type;

	int room_num;
	char nickname[STRSIZE];
}t_leave;

typedef struct{
	unsigned short length;
	unsigned short type;

	int room_num;

	char nickname[20];
	char str[STRSIZE - 20];
}t_chat;