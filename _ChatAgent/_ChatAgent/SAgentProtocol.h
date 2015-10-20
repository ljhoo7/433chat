#pragma once

#include "Common.h"

enum sag_pkt_type
{
	// S -> AG
	pt_tell_agent_number,
	pt_room_info_changed,
	pt_user_info_changed,
	pt_interserver_connected,
	pt_total_room_info,
	pt_total_user_info,
	pt_total_interserver_info,

	// AG -> S
	pt_user_out,
	pt_room_destroy,
	pt_interserver_connect,
	pt_interserver_disconnect
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

typedef struct{
	unsigned short type;
	unsigned short serverNum;
	bool isConnected;
}sag_interserver_connected;

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

typedef struct{
	unsigned short type;
	unsigned short serverCnt;
	unsigned short serverNumList[MAX_CONNECTED_SERVER];
}sag_total_interserver_info;

typedef struct{
	unsigned short type;
	unsigned short agentNum;
}sag_tell_agent_number;

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
	unsigned short serverNum;
}ags_interserver_connect;

typedef struct{
	unsigned short type;
	unsigned short serverNum;
}ags_interserver_disconnect;




