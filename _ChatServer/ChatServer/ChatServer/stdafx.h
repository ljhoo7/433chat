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
class TcpAct;

#include "Act.h"
#include "Proactor.h"
#include "Actor.h"
#include "Cli.h"



