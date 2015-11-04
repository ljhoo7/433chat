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

#define ROOM_MAX 1000
#define TOTAL_PLAYER 10000
#define MAX_CONNECTED_SERVER 5

// one room
#define PLAYER_MAX 10
#define HEADER_SIZE 2

#define NICK_SIZE		20

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
class Connector;
class TcpListenSocket;
class TcpSocket;

class WinSockBase;
class TcpClientServer;

class InterSocket;
class TcpInterServer;

class AgentSocket;
class TcpAgentServer;

#include "ClientProtocol.h"
#include "AgentProtocol.h"
#include "InterProtocol.h"

#include "MemPooler.h"

#include "Acceptor.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"
#include "Connector.h"

#include "TcpAct.h"
#include "TcpListenSocket.h"
#include "TcpSocket.h"

#include "WinSockBase.h"
#include "TcpClientServer.h"

#include "PlayerSocket.h"
#include "Room.h"
#include "RoomManager.h"
#include "Packet.h"
#include "LogicHandle.h"

#include "InterSocket.h"
#include "TcpInterServer.h"

#include "AgentSocket.h"
#include "TcpAgentServer.h"