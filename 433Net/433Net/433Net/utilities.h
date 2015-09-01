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

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg);

// ���� �Լ� ���� ���
void err_display(char *msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);
