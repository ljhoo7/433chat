#include "stdafx.h"

ChatServer* chatServer;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3){
		PRINT("error command\n");
		return 0;
	}

	WORD agentPORT = atoi(argv[2]);
	WORD serverNum = atoi(argv[1]);
	chatServer = new ChatServer(serverNum, agentPORT);

	chatServer->Start();
	return 0;
}

