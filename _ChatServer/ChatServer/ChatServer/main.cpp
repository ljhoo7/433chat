#include "stdafx.h"

int identifier_seed;
// user presence
std::list<CPlayer*> g_vPlayers;

CRoomManager roomManager;
CLogicHandle logicHandle;

TcpClientServer* g_clientServer;
TcpInterServer* g_listenServer;
TcpInterServer* g_connectServer;

int _tmain(int argc, _TCHAR* argv[])
{

	if (argc != 3){
		printf("error command\n");
		return 0;
	}

	WORD listenPort = atoi(argv[1]);
	WORD connectPort = atoi(argv[2]);


	g_listenServer = new TcpInterServer(listenPort, 10);
	g_connectServer = new TcpInterServer(connectPort, 10);
	g_clientServer = new TcpClientServer(listenPort + 1000, 10, 3000);
	identifier_seed = 0;

	std::thread logic_thread(&CLogicHandle::process, &logicHandle);
	g_clientServer->Start();

	g_listenServer->Start(false);
	g_connectServer->Start(true);

	while (true)
	{
		std::string input;
		//ZeroMemory(temp, sizeof(temp));
		std::getline(std::cin, input);

		if (input == "connect")
		{
			g_connectServer->Connect("127.0.0.1", connectPort);
		}
		else if (input == "disconnect")
		{
			if (g_connectServer->isUse) g_connectServer->socket->Disconnect();
			if (g_listenServer->isUse) g_listenServer->socket->Disconnect();
		}

		if (input == "show")
		{
			std::list<CPlayer*>::iterator iter;
			printf("\n--------------------------player info----------------------\n");
			for (iter = g_vPlayers.begin(); iter != g_vPlayers.end(); iter++)
			{
				CPlayer *p = (*iter);
				printf("is Mine : %d, client socket : %d, room Num : %d, nickname : %s\n",
					p->isMine, p->Socket_, p->roomNum, p->nickname.c_str());
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

