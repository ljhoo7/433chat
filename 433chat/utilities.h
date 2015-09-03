#pragma once

#pragma comment(lib, "ws2_32")

#define FD_SETSIZE 1024

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <climits>
#include <WinSock2.h>
#include <ctime>
#include <thread>
#include <future>
#include <exception>
#include <stdexcept>

#define SERVERIP   "127.0.0.1"

#define SERVERPORT1 9000
#define SERVERPORT2 9001

#define ROOM_MAX	1000
#define USER_MAX	100

#include "Protocol.h"
#include "State.h"
#include "StateMachine.h"

// For sending some data through the argument of thread function
typedef struct thread_data
{
	SOCKET		sock;
	int			room_num;
	char*		pkt;
}thread_data;

// Print Socket Error and quit
void err_quit(char *msg);

// Print Socket Error
void err_display(char *msg);

// Customized packet receiving function
int recvn(SOCKET s, char *buf, int len, int flags);

// Get The last readable thread error message
static CHAR* getLastErrorText(CHAR *pBuf, ULONG bufSize);

void printFailSignal(const fail_signal fail);