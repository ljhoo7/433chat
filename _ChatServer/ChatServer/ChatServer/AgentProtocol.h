#pragma once

enum sag_pkt_type
{
	// S -> AG
	pt_room_info_changed,
	pt_user_info_changed,
	//pt_server_info_changed,
	pt_total_room_info,
	pt_total_user_info,
	//pt_total_interserver_info,
	pt_health_ack,

	// AG -> S
	pt_user_out,
	pt_room_destroy,
	pt_kill_server,
	pt_health_check
};
/*
* Server -> Agent
*/


typedef struct
{
	unsigned short type;
	unsigned short roomNum;
	bool isState;
}sag_room_info_changed;

typedef struct
{
	unsigned short type;
	int clientSocket;
	unsigned short roomNum;
	char userName[NICK_SIZE];
	char isConnected;
}sag_user_info_changed;

struct SAGRoomInfo
{
	unsigned short roomNum;
};

typedef struct
{
	unsigned short type;
	unsigned short roomCnt;
	SAGRoomInfo roomInfoList[ROOM_MAX];
}sag_total_room_info;

struct SAGUserInfo
{
	unsigned short roomNum;
	int userSocket;
	char userName[NICK_SIZE];
};

typedef struct
{
	unsigned short type;
	unsigned short userCnt;
	SAGUserInfo userInfoList[TOTAL_PLAYER];
}sag_total_user_info;


typedef struct
{
	unsigned short type;
}sag_health_ack;

/*
* Agent -> Server
*/

typedef struct
{
	unsigned short type;
	int userSocket;
}ags_user_out;

typedef struct
{
	unsigned short type;
	unsigned short roomNum;
}ags_room_destroy;

typedef struct
{
	unsigned short type;
}ags_kill_server;

typedef struct
{
	unsigned short type;
}ags_health_check;



