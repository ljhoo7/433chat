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

class WinSockBase;
class Actor;
class Proactor;

//struct CSockInfo;
class TcpSocket;
class SASocket;
class MSASocket;

class Act;
class TcpAct;
class Acceptor;
class Connector;
class Disconnector;
class CReceiver;
class CSender;
class CLogWriter;
class ServerAgent;
class AgentApp;

#define HEADER_SIZE			2
#define BUFSIZE				8192
#define ELAPSED_DEST		1500



typedef struct mswsock_s {
	LPFN_CONNECTEX ConnectEx;
}mswsock_s;


#include "WinSockBase.h"
#include "SAgentProtocol.h"
#include "MSAgentProtocol.h"

#include "Utilities.h"

#include "Act.h"
#include "Actor.h"
#include "TcpAct.h"
#include "TcpSocket.h"
#include "SASocket.h"


#include "Proactor.h"


//#include "LogWriter.h"

#define PRINTF printf

#include "TcpListenSocket.h"
#include "Connector.h"
#include "Acceptor.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"

#include "ServerAgent.h"
#include "MServerAgent.h"
#include "MSASocket.h"
#include "AgentApp.h"
