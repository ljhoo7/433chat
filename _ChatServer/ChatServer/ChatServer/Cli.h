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

#include "ClientProtocol.h"
#include "MemPooler.h"

class CPlayer;
class CReceiver;

class CPacket;
class CUserToken;

class CBufPoolManager;
class CPacketPoolManager;

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
class TcpServer;

#include "Acceptor.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"

#include "TcpAct.h"
#include "TcpListenSocket.h"
#include "TcpSocket.h"

#include "WinSockBase.h"
#include "TcpServer.h"