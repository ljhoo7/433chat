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

		else if (input == "show")
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
		else if (input == "quit")
		{
			break;
		}
		else if (input.substr(0, 3) == "out")
		{
			int tRoomNum = -1;
			char nickname[20];

			strcpy(nickname, "");

			int s_result = sscanf(input.c_str(), "out %d %s\n", &tRoomNum, nickname);

			if (tRoomNum < 0)
			{
				std::cout << "RoomNum Error !" << std::endl;
				continue;
			}

			if (!strcmp(nickname, ""))
			{
				std::cout << "NickName Error !" << std::endl;
			}
			
			CRoom *tRoom = roomManager.FindRoom(tRoomNum);
			std::list<CPlayer*>::iterator tIter = tRoom->players.begin();
			for (; tIter != tRoom->players.end(); ++tIter)
			{
				if (!strcmp(nickname, (*tIter)->nickname.c_str()))
				{
					t_user_out tOut;

					tOut.type = pkt_type::pt_user_out;
					tOut.client_socket = (*tIter)->socket_;

					std::cout << "out message has been sent to !" << std::endl;
					(*tIter)->Send((char*)&tOut, sizeof(tOut));
				}
			}
		}
		else if (input.substr(0, 6) == "escape")
		{
			char ip[20];
			int port = -1;

			strcpy(ip, "");

			int s_result = sscanf(input.c_str(), "escape %s %d\n", ip, &port);

			if (!strcmp(ip, ""))
			{
				std::cout << "IP Error !" << std::endl;
			}

			if (1025 > port || port > 65535)
			{
				std::cout << "port Error !" << std::endl;
			}

			t_escape_server tEscape;

			tEscape.type = pkt_type::pt_escape_server;
			tEscape.dest_ip = inet_addr(ip);
			tEscape.port = port;

			std::cout << "escape message has been sent to all ! ip :" << tEscape.dest_ip << ", port : " << tEscape.port << std::endl;

			// escaping all
			std::list<CPlayer*>::iterator tIter = players.begin();
			for (; tIter != players.end(); ++tIter)
			{
				(*tIter)->Send((char*)&tEscape, sizeof(tEscape));
			}
		}
	}

	logic_thread.join();

	return 0;
}

