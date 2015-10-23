#pragma once

#include "Common.h"

enum msag_pkt_type
{
	// AG -> MS
	pkt_total_room_info,
	pkt_total_server_count,
	pkt_total_user_info,
	pkt_user_out_success,
	pkt_user_out_fail,
	pkt_room_destroy_success,
	pkt_room_destroy_fail,
	pkt_kill_server_success,
	pkt_kill_server_fail,
	
	// MS -> AG
	pkt_user_out,
	pkt_room_destroy,
	pkt_kill_server,
	pkt_total_request,
};

/*
* Agent -> Monitoring Server
*/

#pragma pack(1)
typedef struct{
	unsigned short type;
	unsigned short roomCnt;
	RoomInfo roomInfoList[MAX_TOTAL_ROOM];
}agms_total_room_info;

#pragma pack(1)
typedef struct{
	unsigned short type;
	unsigned short serverCnt;
}agms_total_server_count;

#pragma pack(1)
typedef struct{
	unsigned short type;
	int serverNum;
	unsigned short userCnt;
	UserInfo userInfoList[MAX_TOTAL_USER];
}agms_total_user_info;

#pragma pack(1)
typedef struct{
	unsigned short type;
}agms_user_out_success;

#pragma pack(1)
typedef struct{
	unsigned short type;
	unsigned short failSignal;
}agms_user_out_fail;

#pragma pack(1)
typedef struct{
	unsigned short type;
}agms_room_destroy_success;

#pragma pack(1)
typedef struct{
	unsigned short type;
	unsigned short failSignal;
}agms_room_destroy_fail;

#pragma pack(1)
typedef struct{
	unsigned short type;
}agms_kill_server_success;

#pragma pack(1)
typedef struct{
	unsigned short type;
	unsigned short failSignal;
}agms_kill_server_fail;


/*
* Monitoring Server -> Agent
*/

#pragma pack(1)
typedef struct{
	unsigned short type;
	int serverNum;
	int userSocket;
}msag_user_out;

#pragma pack(1)
typedef struct{
	unsigned short type;
	unsigned short roomNum;
}msag_room_destroy;

#pragma pack(1)
typedef struct{
	unsigned short type;
	int serverNum;
}msag_kill_server;

#pragma pack(1)
typedef struct{
	unsigned short type;
}msag_request_total_info;

