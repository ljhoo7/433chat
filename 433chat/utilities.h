//////////////////////////////////////////////////////////////
// ���� ��ƿ��Ƽ�� ����

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

// ���� ���� ������ ���� ����ü�� ����
struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
};

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg);

// ���� �Լ� ���� ���
void err_display(char *msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);