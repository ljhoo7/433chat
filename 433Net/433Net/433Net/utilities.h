//////////////////////////////////////////////////////////////
// ���� ��ƿ��Ƽ�� ����

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


// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg);

// ���� �Լ� ���� ���
void err_display(char *msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);
