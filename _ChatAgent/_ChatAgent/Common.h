#pragma once

#include "stdafx.h"

#define NICK_SIZE 20
#define MAX_TOTAL_ROOM 100
#define MAX_TOTAL_USER 1000
#define MAX_CONNECTED_SERVER 5

struct RoomInfo
{
	unsigned short roomNum;
};

struct UserInfo
{
	unsigned short				 roomNum;
	int							 userSocket;
	char						 userName[NICK_SIZE];

	UserInfo():roomNum(-1), userSocket(0)
	{
		strcpy_s(userName, NICK_SIZE, "");
	}
	~UserInfo(){}
};

struct ServerInfo
{
	unsigned int		serverNum;
	unsigned short		userCount;
	std::list<UserInfo> userInfoList;

	ServerInfo() : userCount(0){}
	~ServerInfo(){ if(!userInfoList.empty()) userInfoList.clear(); }

	bool operator==(const ServerInfo& other)
	{
		return (this->serverNum == other.serverNum) && 
			(this->userCount == other.userCount);
	}
};

struct TotalInfo
{
	unsigned short			      roomCnt;
	std::list<RoomInfo>			  roomInfoList;

	std::list<ServerInfo>		  serverUserInfoList;

	unsigned short			      serverCnt;
	std::list<unsigned short>	  serverNumList;

	TotalInfo() : roomCnt(0),serverCnt(0){}
	~TotalInfo(){}
};


enum fail_signal
{
	fs_alreadyexist,
	fs_overflow,
	fs_no_exist
};
