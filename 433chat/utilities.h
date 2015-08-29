//////////////////////////////////////////////////////////////
// 여러 유틸리티들 모음

#pragma once

#pragma comment(lib, "ws2_32")

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <WinSock2.h>

#include "protocol.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
};

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg);

// 소켓 함수 오류 출력
void err_display(char *msg);

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);