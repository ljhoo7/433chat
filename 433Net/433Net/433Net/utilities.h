#pragma once
#pragma comment(lib, "ws2_32")

#define SERVERIP   "127.0.0.1"
#define CONNECTIP "127.0.0.1"

#define FD_SETSIZE 1024
#define BUFSIZE 1024
#define ROOM_MAX 100
#define TOTAL_PLAYER 1000

#define PLAYER_MAX 10 // one room
#define HEADER_SIZE 2

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <climits>
#include <WinSock2.h>
#include <stdio.h>
#include <thread>
#include <list>
#include <queue>

#include "protocol.h"

class Receiver;
class IPeer;
class UserToken;
class Packet;
class BufPoolManager;
class PacketPoolManager;
class LogicHandle;
class InterServer;

// socket func error print and exit
void err_quit(char *msg);

// socket func error print only
void err_display(char *msg);

// custom define recieve func
int recvn(SOCKET s, char *buf, int len, int flags);
