#include "stdafx.h"

int identifier_seed;
// user presence
std::list<CPlayer*> g_vPlayers;

CRoomManager roomManager;
CLogicHandle logicHandle;

TcpClientServer server(10000, 10, 3000);
TcpInterServer listen_server(10001, 10);
TcpInterServer connect_server(10002, 10);

int _tmain(int argc, _TCHAR* argv[])
{
	
	identifier_seed = 0;

	std::thread logic_thread(&CLogicHandle::process, &logicHandle);
	server.Start();

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

	logic_thread.join();

	return 0;
}

