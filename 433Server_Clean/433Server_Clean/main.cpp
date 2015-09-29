
#include "stdafx.h"

CInterServer listen_server;
CInterServer connect_server;
CLogicHandle logicHandle;
CReceiver	receiver;

CRoomManager roomManager;

int identifier_seed;

// user presence
std::list<CPlayer*> g_vPlayers;

void accept_callback(CUserToken* token)
{
	CPlayer* p = new CPlayer(true);

	g_vPlayers.push_back(p);

	token->peer = p;
	p->token = token;

	ss_connect tmpConnect;
	tmpConnect.type = ssType::pkt_connect;
	tmpConnect.client_socket = p->token->clientSocket;

	if (listen_server.the_other_sock == NULL)
	{
		connect_server._send((char *)&tmpConnect, sizeof(ss_connect));
	}
	else
	{
		listen_server._send((char *)&tmpConnect, sizeof(ss_connect));
	}
	/* connect msg send */
	return;
}

int _tmain(int argc, _TCHAR* argv[])
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

	std::thread logic_thread(&CLogicHandle::process, &logicHandle);

	receiver.start(client_port, accept_callback);

	std::thread client_thread(&CReceiver::process, &receiver);

	while (true)
	{
		std::string input;
		//ZeroMemory(temp, sizeof(temp));
		std::getline(std::cin, input);

		if (listen_server.the_other_sock == NULL)
		{
			if (input == "connect")
			{
				connect_server.start(0, connect_port);
			}
			else{
				if (input == "disconnect")
				{
					connect_server.disconnect();
				}
			}
		}

		if (connect_server.the_other_sock == NULL)
		{
			if (input == "disconnect")
			{
				listen_server.disconnect();
			}
		}
		if (input == "show")
		{
			std::list<CPlayer*>::iterator iter;
			printf("\n--------------------------player info----------------------\n");
			for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
			{
				CPlayer *p = (*iter);
				printf("is Mine : %d, client socket : %d, room Num : %d, nickname : %s\n",
					p->isMine, p->token->clientSocket, p->roomNum, p->nickname.c_str());
			}
			printf("-----------------------------------------------------------\n");
		}
		if (input == "quit")
		{
			break;
		}
	}

	if (connect_server.process_thread.joinable())
		connect_server.process_thread.join();
	if (listen_server.listen_thread.joinable())
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

