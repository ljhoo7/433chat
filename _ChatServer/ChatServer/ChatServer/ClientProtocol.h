#pragma once

#define NICK_SIZE		20

enum pkt_type {
	pt_create,
	pt_create_success,
	pt_create_failure,
	pt_destroy,
	pt_destroy_success,
	pt_destroy_failure,
	pt_join,
	pt_join_success,
	pt_join_failure,
	pt_leave,
	pt_leave_success,
	pt_chat,
	pt_chat_alarm,
	pt_join_alarm,
	pt_leave_alarm,
	pt_kick,
	pt_user_out_client,
	pt_escape_server,
	pt_escape_success,
	pt_escape_failure,
	pt_default
};

enum fail_signal
{
	fs_alreadyexist,
	fs_overflow,
	fs_no_exist
};

typedef struct{
	unsigned short type;
	unsigned short room_num;
}t_create;

typedef struct{
	unsigned short type;
}t_create_success;

typedef struct{
	unsigned short type;
	unsigned short fail_signal;
}t_create_failure;

typedef struct{
	unsigned short type;
	unsigned short room_num;
}t_destroy;

typedef struct{
	unsigned short type;
}t_destroy_success;

typedef struct{
	unsigned short type;
	unsigned short fail_signal;
}t_destroy_failure;

typedef struct{
	unsigned short type;
	unsigned short room_num;
	char nickname[NICK_SIZE];
}t_join;

typedef struct{
	unsigned short	type;
	unsigned short	trash_value;		// It is used because of no packing.
	unsigned int	token;
}t_join_success;

typedef struct{
	unsigned short type;
	unsigned short fail_signal;
}t_join_failure;

typedef struct{
	unsigned short type;
	unsigned short room_num;
	unsigned int token;
	char nickname[NICK_SIZE];
}t_leave;

typedef struct{
	unsigned short type;
}t_leave_success;

typedef struct{
	unsigned short type;
	unsigned short trashValue;
	int client_socket;
}t_user_out;

typedef struct{
	unsigned short type;
	unsigned short trashValue;
	unsigned long dest_ip;
	unsigned short port;
	unsigned short trashValue2;
}t_escape_server;

typedef struct{
	unsigned short type;
	int client_sock;
}t_escape_success;

typedef struct{
	unsigned short type;
}t_escape_failure;

typedef struct{
	unsigned short type;
	unsigned short length;
	//unsigned short trash_value;
	unsigned short room_num;
	unsigned int token;
	char nickname[NICK_SIZE];
}t_chat;

typedef struct{
	unsigned short type;
	unsigned short length;
	//unsigned short trash_value;
	unsigned short room_num;
	unsigned int token;
	char nickname[NICK_SIZE];
}t_chat_alarm;

typedef struct{
	unsigned short type;
	unsigned short room_num;
	char nickname[NICK_SIZE];
}t_join_alarm;

typedef struct{
	unsigned short type;
	unsigned short room_num;
	char nickname[NICK_SIZE];
}t_leave_alarm;

typedef struct{
	unsigned short type;
}t_kick;