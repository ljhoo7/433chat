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

struct CProactor;

class CActor;

struct CSockInfo;
struct CAct;

class CClient;

class CConnector;
class CDisconnector;
class CReceiver;
class CSender;

class CLogWriter;

#define BUFSIZE		1024

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

#include "Proactor.h"

#include "Actor.h"
#include "Act.h"

#include "LogWriter.h"

#include "Client.h"

#include "Connector.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"
