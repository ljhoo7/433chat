#pragma once

#pragma comment(lib, "ws2_32")
#pragma comment(lib,"mswsock.lib")  

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 8192
#define MAXSERVER 100

#ifdef _DEBUG
	#define MYDEF
#else
#endif

class Actor;
class Proactor;
class TcpAct;

#include "targetver.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <wchar.h>

class CLogWriter;
struct ServerInfo;
class ChatServer;

extern ChatServer* chatServer;
#define PRINTF printf

#include "LogAct.h"
#include "LogWriter.h"

#include "Act.h"
#include "Proactor.h"
#include "Actor.h"

#include "Def.h"
#include "ChatServer.h"


