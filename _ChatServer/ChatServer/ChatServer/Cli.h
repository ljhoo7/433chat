#pragma once

#include <cstdio>
#include <tchar.h>
#include <list>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <climits>
#include <thread>
#include <queue>
#include <Windows.h>
#include <process.h>

#define ROOM_MAX 100
#define TOTAL_PLAYER 1000

// one room
#define PLAYER_MAX 10
#define HEADER_SIZE 2

class CPlayer;

class CPacket;
class CUserToken;

class CLogicHandle;

class CRoomManager;
class CRoom;

class Acceptor;
class Receiver;
class Sender;
class Disconnector;
class TcpListenSocket;
class TcpSocket;

class WinSockBase;
class TcpClientServer;

#include "ClientProtocol.h"
#include "MemPooler.h"

#include "Acceptor.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"

#include "TcpAct.h"
#include "TcpListenSocket.h"
#include "TcpSocket.h"

#include "WinSockBase.h"
#include "TcpClientServer.h"

#include "Player.h"
#include "Room.h"
#include "RoomManager.h"
#include "Packet.h"
#include "LogicHandle.h"
