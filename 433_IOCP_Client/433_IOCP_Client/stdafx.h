#pragma once

#pragma comment (lib, "ws2_32")
#pragma comment (lib, "Mswsock")

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <cstdio>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <string>
#include <iostream>
#include <winnt.h>
#include <vector>
#include <MSWSock.h>

struct CProactor;

class CActor;

struct CSockInfo;
class CAct;

struct CClient;

class CConnector;
class CDisconnector;
class CReceiver;
class CSender;

class CLogWriter;

template <class entity_type>
class StateMachine;

template <class entity_type>
class State;

#define HEADER_SIZE			2
#define BUFSIZE				8192
#define ELAPSED_DEST		1500

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

#include "SockInfo.h"

#include "Proactor.h"

#include "Actor.h"
#include "Act.h"

#include "LogWriter.h"

#include "Client.h"
#include "ClientStates.h"

#include "Connector.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"
