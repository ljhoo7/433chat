#pragma once

#pragma comment (lib, "ws2_32")

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

class CProactor;

class CActor;

struct CSockInfo;
struct CAct;

class CBotConnector;
class CBotReceiver;
class CBotSender;
class CLogWriter;

#define BUFSIZE 1024

#ifdef _DEBUG
	#define MYDEF
#else
#endif

typedef struct mswsock_s {
	LPFN_CONNECTEX ConnectEx;
}mswsock_s;

#include "Protocol.h"
#include "Utilities.h"

#include "SockInfo.h"
#include "Act.h"

#include "Proactor.h"

#include "Actor.h"

#include "BotReceiver.h"
#include "BotSender.h"
#include "BotConnector.h"
#include "LogWriter.h"
#include "Bot.h"
