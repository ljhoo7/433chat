#pragma once

#pragma comment(lib, "ws2_32")
#pragma comment(lib,"mswsock.lib")  

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUFSIZE 8192

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

extern CLogWriter* logWriter;
#define PRINTF logWriter->myWPRINTF

#include "LogAct.h"
#include "LogWriter.h"

#include "Act.h"
#include "Proactor.h"
#include "Actor.h"

#include "Def.h"



