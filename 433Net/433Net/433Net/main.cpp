#pragma once
#include "433Net.h"
#include "Player.h"
#include "RoomManager.h"

int nTotalSockets = 0;
int g_nIsListen;

// ip
unsigned long	g_nIp;
// port
int				g_nPort;
Reciever* server;

RoomManager roomManager;

void accept_callback(UserToken* token){
	Player* p = new Player();
	token->peer = p;
	p->token = *token;
	return;
}

DWORD WINAPI InterServerThread(LPVOID arg);

void remove_callback(UserToken* token){
	/* 메모리 누수 막아야댐 이거 좀있다 하자! */
}

DWORD WINAPI recieve_process(LPVOID arg){
	server->process();
	return 0;
}
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		puts("usage : (program_name) (0=listen,1=connect)\n");
		return 0;
	}
	
	g_nIsListen = atoi(argv[1]);

	if (g_nIsListen != 0 && g_nIsListen != 1)
	{
		puts("The second argument must be zero or one.\n");
	}

	// 리시브 매니저
	server = new Reciever();
	server->start(9000, 100, accept_callback);
	
	HANDLE hHandle[2];
	hHandle[0] = CreateThread(NULL, 0, recieve_process, 0, 0, NULL);
	hHandle[1] = CreateThread(NULL, 0, InterServerThread, 0, 0, NULL);
	// 윈속 종료

	WaitForMultipleObjects(2, hHandle, TRUE, INFINITE);
	CloseHandle(hHandle[0]);
	CloseHandle(hHandle[1]);
	WSACleanup();
	return 0;
}

