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

#define SERVERIP   "127.0.0.1"
#pragma once

#pragma comment(lib, "ws2_32")


#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512
#define ROOM_MAX   99999


// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg);

// 소켓 함수 오류 출력
void err_display(char *msg);

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);
