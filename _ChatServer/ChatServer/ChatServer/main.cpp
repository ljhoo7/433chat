#include "stdafx.h"

int identifier_seed;
// user presence
std::list<CPlayer*> players;

CRoomManager roomManager;
CLogicHandle logicHandle;

TcpClientServer* clientServer;
TcpInterServer* listenServer;
TcpInterServer* connectServer;

CLogWriter *logWriter;

int _tmain(int argc, _TCHAR* argv[])
{

	if (argc != 3){
		PRINTF(L"error command\n");
		return 0;
	}
	logWriter = new CLogWriter(L"BotLog.log", 2);

	WORD listenPort = _wtoi(argv[1]);
	WORD connectPort = _wtoi(argv[2]);


	listenServer = new TcpInterServer(listenPort, 10);
	connectServer = new TcpInterServer(connectPort, 10);
	clientServer = new TcpClientServer(listenPort + 1000, 10, 3000);
	identifier_seed = 0;

	std::thread logic_thread(&CLogicHandle::Process, &logicHandle);
	clientServer->Start();

	listenServer->Start(false);
	connectServer->Start(true);

	while (true)
	{
		std::string input;
		//ZeroMemory(temp, sizeof(temp));
		std::getline(std::cin, input);

		if (input == "connect")
		{
			connectServer->Connect("127.0.0.1", connectPort);
		}
		else if (input == "disconnect")
		{
			if (connectServer->isUse) connectServer->socket->Disconnect();
			if (listenServer->isUse) listenServer->socket->Disconnect();
		}

		if (input == "show")
		{
			std::list<CPlayer*>::iterator iter;
			PRINTF(L"\n--------------------------player info----------------------\n");
			for (iter = players.begin(); iter != players.end(); iter++)
			{
				CPlayer *p = (*iter);
				PRINTF(L"is Mine : %d, client socket : %d, room Num : %d, nickname : %s\n",
					p->isMine, p->socket_, p->roomNum, p->nickname.c_str());
			}
			PRINTF(L"-----------------------------------------------------------\n");
		}
		if (input == "quit")
		{
			break;
		}
	}

	logic_thread.join();

	return 0;
}

