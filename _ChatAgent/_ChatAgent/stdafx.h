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
#include <list>
#include <algorithm>
#include <MSWSock.h>
#include <assert.h>


class CActor;
struct CProactor;

//struct CSockInfo;
class TcpSocket;
class SASocket;
class MSASocket;

class CAct;
class TcpAct;

class CConnector;
class CDisconnector;
class CReceiver;
class CSender;

class CLogWriter;

template <class entity_type>
class StateMachine;

template <class entity_type>
class State;

class ServerAgent;

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


#include "MSAgentProtocol.h"
#include "SAgentProtocol.h"

#include "Utilities.h"

//#include "SockInfo.h"
#include "Act.h"
#include "Actor.h"
#include "TcpAct.h"
#include "TcpSocket.h"
#include "SASocket.h"


#include "Proactor.h"


#include "LogWriter.h"
class CLogWriter;
extern CLogWriter* g_pLog;

#define PRINTF printf


#include "Connector.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"

#include "ServerAgent.h"
#include "MServerAgent.h"
#include "MSASocket.h"
