#pragma once
#include "433Net.h"
#include "Player.h"
#include "RoomManager.h"

int nTotalSockets = 0;

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

void remove_callback(UserToken* token){
	/* �޸� ���� ���ƾߴ� �̰� ���ִ� ����! */
}

DWORD WINAPI recieve_process(LPVOID arg){
	server->process();
	return 0;
}
int main(int argc, char *argv[])
{
	// ���ú� �Ŵ���
	server = new Reciever();
	server->start(9000, 100, accept_callback);
	
	HANDLE hHandle[1];
	hHandle[0] = CreateThread(NULL, 0, recieve_process, 0, 0, NULL);
	// ���� ����

	WaitForMultipleObjects(1, hHandle, TRUE, INFINITE);
	CloseHandle(hHandle[0]);
	WSACleanup();
	return 0;
}

