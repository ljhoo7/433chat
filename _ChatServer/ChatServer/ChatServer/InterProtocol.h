#pragma once

#define PKTLEN 1024
#define STRSIZE	1024
#define NICK_SIZE 20

enum ssType
{
	pkt_create,
	pkt_destroy,
	pkt_join,
	pkt_leave,
	pkt_chat,
	pkt_connect,
	pkt_disconnect,
	pkt_heartbeats,
	pkt_heartbeats_response,
	pkt_room_info_send,
	pkt_player_info_send,
	pkt_room_info_success,
	pkt_room_info_failure,
	pkt_player_info_success,
	pkt_player_info_failure,
	pkt_sync_req,
};

typedef struct
{
	unsigned short type;
} ss_sync_req;

typedef struct
{
	unsigned short type;
	int client_socket;
	unsigned short room_num;
} ss_create;

typedef struct
{
	unsigned short type;
	int client_socket;
	unsigned short room_num;
} ss_destroy;

typedef struct
{
	unsigned short type;
	int client_socket;
	unsigned short room_num;
	char nickname[NICK_SIZE];
} ss_join;

typedef struct
{
	unsigned short type;
	int client_socket;
	unsigned short room_num;
	char nickname[NICK_SIZE];
	unsigned int token;
} ss_leave;

typedef struct
{
	unsigned short type;
	unsigned short length;
	unsigned short room_num;
	unsigned int token;
	char nickname[NICK_SIZE];
} ss_chat;



typedef struct
{
	unsigned short type;
	int client_socket;
	int server_num;
} ss_connect;

typedef struct
{
	unsigned short type;
	int client_socket;
	int server_num;
} ss_disconnect;

typedef struct
{
	unsigned short type;
} ss_heartbeats;

typedef struct
{
	unsigned short type;
} ss_heartbeats_response;

typedef struct
{
	unsigned short room_num;
} room_info;

typedef struct
{
	unsigned short type;
	unsigned short length;
	unsigned short room_cnt;
} ss_room_info_send;

typedef struct
{
	int client_socket;
	int server_num;
	short room_num;
	char nickname[20];
	unsigned int token;
} player_info;

typedef struct
{
	unsigned short type;
	// not include type size(2byte)
	unsigned short length;
	unsigned short player_cnt;
} ss_player_info_send;

typedef struct
{
	unsigned short type;
} ss_room_info_success;

typedef struct
{
	unsigned short type;
} ss_room_info_failure;

typedef struct
{
	unsigned short type;
} ss_player_info_success;

typedef struct
{
	unsigned short type;
} ss_player_info_failure;
