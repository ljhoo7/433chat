#pragma once

#include "Common.h"

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
typedef struct{
	unsigned short type;
	unsigned short roomNum;
	bool isState;	
}sag_room_info_changed;

typedef struct{
	unsigned short type;
	int clientSocket;
	unsigned short roomNum;
	char userName[NICK_SIZE];
	char isConnected;
}sag_user_info_changed;

//typedef struct{
//	unsigned short type;
//	unsigned short serverNum;
//	bool isConnected;
//}sag_server_info_changed;

typedef struct{
	unsigned short type;
	unsigned short roomCnt;
	RoomInfo roomInfoList[MAX_TOTAL_ROOM];
}sag_total_room_info;

typedef struct{
	unsigned short type;
	unsigned short userCnt;
	UserInfo userInfoList[MAX_TOTAL_USER];
}sag_total_user_info;

//typedef struct{
//	unsigned short type;
//	unsigned short serverCnt;
//	unsigned short serverNumList[MAX_CONNECTED_SERVER];
//}sag_total_interserver_info;

typedef struct{
	unsigned short type;
}sag_health_ack;

/*
* Agent -> Server
*/

typedef struct{
	unsigned short type;
	int userSocket;
}ags_user_out;

typedef struct{
	unsigned short type;
	unsigned short roomNum;
}ags_room_destroy;

typedef struct{
	unsigned short type;
}ags_kill_server;

typedef struct{
	unsigned short type;
}ags_health_check;






