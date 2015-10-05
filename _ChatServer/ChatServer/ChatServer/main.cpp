#include "stdafx.h"

int identifier_seed;
// user presence
std::list<CPlayer*> g_vPlayers;

CRoomManager roomManager;
CLogicHandle logicHandle;

int _tmain(int argc, _TCHAR* argv[])
{
	TcpClientServer server(10000, 10, 3000);
	identifier_seed = 0;

	std::thread logic_thread(&CLogicHandle::process, &logicHandle);
	server.Start();

	while (true)
	{

	}

	logic_thread.join();

	return 0;
}

