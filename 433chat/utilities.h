//////////////////////////////////////////////////////////////
// 여러 유틸리티들 모음

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

#define SERVERIP   "127.0.0.1"
#define SERVERPORT1 9000
#define SERVERPORT2 9001
#define BUFSIZE		512
#define ROOM_MAX	99999

#include "protocol.h"
#include "State.h"
#include "StateMachine.h"

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
};

// 메세지 뿌리는 스레드에 넘길 자료구조
typedef struct thread_data
{
	SOCKET		sock;
	int			room_num;
	t_packet	pkt;
}thread_data;

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg);

// 소켓 함수 오류 출력
void err_display(char *msg);

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);