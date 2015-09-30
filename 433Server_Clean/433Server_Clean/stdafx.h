#pragma once

#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "targetver.h"

#include <cstdio>
#include <tchar.h>
#include <list>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <climits>
#include <WinSock2.h>
#include <thread>
#include <queue>

class IPeer;
class CPlayer;

class CReceiver;

class CPacket;
class CUserToken;

class CBufPoolManager;
class CPacketPoolManager;

class CLogicHandle;
class CInterServer;

class CRoomManager;
class CRoom;

#define SERVERIP  "127.0.0.1"
#define CONNECTIP "127.0.0.1"

#define FD_SETSIZE 1024
#define BUFSIZE 1024
#define ROOM_MAX 100
#define TOTAL_PLAYER 1000

// one room
#define PLAYER_MAX 10
#define HEADER_SIZE 2

typedef struct SOCKETINFO
{
	OVERLAPPED	overlapped;
	SOCKET		sock;
	char		recv_buf[BUFSIZE + 1];
	char		send_buf[BUFSIZE + 1];
	int			toReceiveBytes, receivedBytes;
	int			toSendBytes, sentBytes;
	WSABUF		recv_wsabuf;
	WSABUF		send_wsabuf;
	IPeer		*pPeer;
	bool		isChat;
}SOCKETINFO;

typedef struct COMPLEMENT_KEY
{
	SOCKET		sock;
	IPeer		*pPeer;
}COMPLEMENT_KEY;

#include "Protocol.h"
#include "ClientProtocol.h"

#include "Utilities.h"

#include "Packet.h"
#include "Receiver.h"

#include "BufPoolManager.h"
#include "PacketPoolManager.h"
#include "Peer.h"
#include "Player.h"

#include "Usertoken.h"

#include "LogicHandle.h"
#include "InterServer.h"
#include "RoomManager.h"
#include "Room.h"