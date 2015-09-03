#pragma once
#include "utilities.h"
#include "interServer.h"
#include "logic.h"

InterServer listen_server;
InterServer connect_server;
LogicHandle logicHandle;

int main(int argc, char *argv[])
{
	int listen_port;
	int connect_port;
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSAStartup() error!");

	if (argc != 3){
		printf("error command\n");
		return 0;
	}

	listen_port = atoi(argv[1]);
	connect_port = atoi(argv[2]);

	listen_server.start(1, listen_port);

	std::thread logic_thread(&LogicHandle::process, &logicHandle);
	

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
	WSACleanup();
	return 0;
}