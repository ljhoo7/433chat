#pragma once

#pragma comment(lib, "ws2_32")
#pragma comment(lib,"mswsock.lib")  

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "targetver.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <thread>
#include <queue>
#include <Windows.h>
#include <process.h>
#include <assert.h>

class Actor;
class Proactor;
class Acceptor;
class Receiver;
class Sender;

class TcpAct;
class TcpListenSocket;
class TcpSocket;


#include "Act.h"
#include "Proactor.h"
#include "Actor.h"

#include "TcpAct.h"
#include "TcpListenSocket.h"
#include "TcpSocket.h"

#include "Acceptor.h"
#include "Disconnector.h"
#include "Receiver.h"
#include "Sender.h"

#include "WinSockBase.h"
#include "TcpServer.h"