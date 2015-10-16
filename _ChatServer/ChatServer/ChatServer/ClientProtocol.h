#pragma once

#define NICK_SIZE		20

enum pkt_type
{
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
	pt_kick
};

enum fail_signal
{
	fs_alreadyexist,
	fs_overflow,
	fs_no_exist
};

typedef struct
{
	unsigned short type;
	unsigned short room_num;
}t_create;
typedef struct
{
	unsigned short type;
}t_create_success;
typedef struct
{
	unsigned short type;
	unsigned short fail_signal;
}t_create_failure;
typedef struct
{
	unsigned short type;
	unsigned short room_num;
}t_destroy;
typedef struct
{
	unsigned short type;
}t_destroy_success;
typedef struct
{
	unsigned short type;
	unsigned short fail_signal;
}t_destroy_failure;
typedef struct
{
	unsigned short type;
	unsigned short room_num;
	char nickname[NICK_SIZE];
}t_join;
typedef struct
{
	unsigned short  type;
	unsigned short	trash_value;		// It is used because of no packing.
	unsigned int	token;
}t_join_success;
typedef struct
{
	unsigned short type;
	unsigned short fail_signal;
}t_join_failure;
typedef struct
{
	unsigned short type;
	unsigned short room_num;
	unsigned int token;
	char nickname[NICK_SIZE];
}t_leave;
typedef struct
{
	unsigned short type;
}t_leave_success;

typedef struct
{
	unsigned short type;
	unsigned short length;
	//unsigned short trash_value;
	unsigned short room_num;
	unsigned int token;
	char nickname[NICK_SIZE];
	std::string message;
}t_chat;

typedef struct
{
	unsigned short type;
	unsigned short length;
	//unsigned short trash_value;
	unsigned short room_num;
	unsigned int token;
	char nickname[NICK_SIZE];
	std::string message;
}t_chat_alarm;

typedef struct
{
	unsigned short type;
	unsigned short room_num;
	char nickname[NICK_SIZE];
}t_join_alarm;

typedef struct
{
	unsigned short type;
	unsigned short room_num;
	char nickname[NICK_SIZE];
}t_leave_alarm;

typedef struct
{
	unsigned short type;
}t_kick;