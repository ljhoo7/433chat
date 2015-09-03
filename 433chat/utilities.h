//////////////////////////////////////////////////////////////
// ���� ��ƿ��Ƽ�� ����

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

// ���� ���� ������ ���� ����ü�� ����
struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
};

// �޼��� �Ѹ��� �����忡 �ѱ� �ڷᱸ��
typedef struct thread_data
{
	SOCKET		sock;
	int			room_num;
	t_packet	pkt;
}thread_data;

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg);

// ���� �Լ� ���� ���
void err_display(char *msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);