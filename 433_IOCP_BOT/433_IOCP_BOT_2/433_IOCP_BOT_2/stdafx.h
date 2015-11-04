#pragma once

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Mswsock")

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <cstdio>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <WinSock2.h>
#include <process.h>
#include <Windows.h>
#include <winnt.h>
#include <MSWSock.h>
#include <thread>
#include <random>

#include <vector>

class CLogWriter;

class CAct;
class CActor;

class CProactor;

class CConnector;
class CDisconnector;

class CReceiver;
class CSender;

class CBot;

template <class entity_type>
class StateMachine;

template <class entity_type>
class State;

#define HEADER_SIZE			2
#define BUFSIZE				8192
#define ELAPSED_DEST		1500
#define ROOM_MAX			1000
#define PLAYER_MAX			10
#define TOTAL_PLAYER		10000

#ifdef _DEBUG
#define MYDEF
#else

#endif

typedef struct mswsock_s {
	LPFN_CONNECTEX ConnectEx;
}mswsock_s;

#include "State.h"
#include "StateMachine.h"

#include "Protocol.h"

#include "Utilities.h"

#include "LogWriter.h"

#include "Act.h"
#include "Actor.h"

#include "BotStates.h"
#include "Bot.h"

#include "Proactor.h"

#include "Connector.h"
#include "Disconnector.h"

#include "Receiver.h"
#include "Sender.h"

