//////////////////////////////////////////////////////////////
// 여러 유틸리티들 모음

#pragma once

#pragma comment(lib, "ws2_32")

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

#define SERVERIP   "127.0.0.1"
#define SERVERPORT1 9000
#define SERVERPORT2 9001
#define BUFSIZE    512
#define ROOM_MAX   99999
#define HEADER_SIZE 2

class Receiver;
class UserToken;
class IPeer;

#include "reciever.h"
#include "usertoken.h"
#include "protocol.h"

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg);

// 소켓 함수 오류 출력
void err_display(char *msg);

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);
