#pragma once

#include "Common.h"

enum msag_pkt_type
{
	// AG -> MS
	pkt_total_room_info,
	pkt_total_user_info,
	pkt_total_interserver_info,
	pkt_request_total_info,
	pkt_interserver_success,
	pkt_interserver_fail,
	
	// MS -> AG
	pkt_user_out,
	pkt_room_destroy,
	pkt_interserver_connect,
	pkt_interserver_disconnect,
	
};

/*
* Agent -> Monitoring Server
*/

typedef struct{
	unsigned short type;
	unsigned short roomCnt;
	RoomInfo roomInfoList[MAX_TOTAL_ROOM];
}agms_total_room_info;

typedef struct{
	unsigned short type;
	unsigned short userCnt;
	UserInfo userInfoList[MAX_TOTAL_USER];
}agms_total_user_info;

typedef struct{
	unsigned short type;
	unsigned short serverCnt;
	unsigned short serverNumList[MAX_CONNECTED_SERVER];
}agms_total_interserver_info;



//typedef struct{
//	unsigned short type;
//
//	// 7
//	unsigned short agentNum;
//
//	// 6
//	unsigned short serverCnt;
//	unsigned short serverNumList[MAX_CONNECTED_SERVER];
//
//	// 4
//	unsigned short roomCnt;
//	RoomInfo roomInfoList[MAX_TOTAL_ROOM];
//
//	// 5
//	unsigned short userCnt;
//	UserInfo userInfoList[MAX_TOTAL_USER];
//
//}agms_total_info_send;

typedef struct{
	unsigned short type;
}agms_pt_interserver_success;

typedef struct{
	unsigned short type;
	unsigned short failSignal;
}agms_pt_interserver_fail;



/*
* Monitoring Server -> Agent
*/

typedef struct{
	unsigned short type;
	int userSocket;
}msag_user_out;

typedef struct{
	unsigned short type;
	unsigned short roomNum;
}msag_room_destroy;

typedef struct{
	unsigned short type;
	unsigned short serverNum;
}msag_interserver_connect;

typedef struct{
	unsigned short type;
	unsigned short serverNum;
}msag_interserver_disconnect;

typedef struct{
	unsigned short type;
}msag_request_total_info;

