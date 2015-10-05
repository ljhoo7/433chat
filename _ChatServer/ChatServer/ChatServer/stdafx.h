#pragma once

#pragma comment(lib, "ws2_32")
#pragma comment(lib,"mswsock.lib")  

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "targetver.h"

#include <WinSock2.h>
#include <MSWSock.h>

class LogWriter;

class Actor;
class Proactor;
class TcpAct;

#include "Act.h"
#include "Proactor.h"
#include "Actor.h"
#include "Def.h"



