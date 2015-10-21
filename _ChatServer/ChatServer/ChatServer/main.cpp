#include "stdafx.h"

ChatServer* chatServer;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3){
		PRINTF("error command\n");
		return 0;
	}

	WORD agentPORT = _wtoi(argv[2]);
	WORD serverNum = _wtoi(argv[1]);
	chatServer = new ChatServer(serverNum, agentPORT);

	chatServer->Start();
	return 0;
}

