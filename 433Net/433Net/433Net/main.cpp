#pragma once
#include "utilities.h"
#include "interServer.h"
#include "logic.h"
#include "reciever.h"
#include "Player.h"
#include "RoomManager.h"

InterServer listen_server;
InterServer connect_server;
LogicHandle logicHandle;
Reciever	receiver;

RoomManager roomManager;

int identifier_seed;

// user presence
std::list<Player*> g_vPlayers;

void accept_callback(UserToken* token){
	Player* p = new Player(true);

	g_vPlayers.push_back(p);

	token->peer = p;
	p->token = *token;
	return;
}

int main(int argc, char *argv[])
{
	int listen_port;
	int connect_port;

	int client_port;

	identifier_seed = 0;
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSAStartup() error!");

	if (argc != 3){
		printf("error command\n");
		return 0;
	}

	listen_port = atoi(argv[1]);
	connect_port = atoi(argv[2]);

	client_port = 2024 + listen_port;

	listen_server.start(1, listen_port);

	std::thread logic_thread(&LogicHandle::process, &logicHandle);

	receiver.start(client_port, accept_callback);

	std::thread client_thread(&Reciever::process, &receiver);

	while (true){
		std::string input;
		//ZeroMemory(temp, sizeof(temp));
		std::getline(std::cin, input);

		if (listen_server.the_other_sock == NULL){
			if (input=="connect"){
				connect_server.start(0, connect_port);
			}
		}
		else{
			if (input == "disconnect"){
				listen_server.disconnect();
			}
		}

		if (connect_server.the_other_sock == NULL){
			if (input == "disconnect"){
				connect_server.disconnect();
			}
		}

		if (input == "quit"){
			break;
		}
	}

	listen_server.listen_thread.join();

	logic_thread.join();
	client_thread.join();

	if (WSACleanup() != 0)
	{
		std::cout << "An error occured on WASCleanup()." << std::endl;
		exit(1);
	}
	return 0;
}