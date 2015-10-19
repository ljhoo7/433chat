#include "stdafx.h"

ChatServer* chatServer;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2){
		PRINTF("error command\n");
		return 0;
	}

	WORD serverNum = _wtoi(argv[1]);
	chatServer = new ChatServer(serverNum);

	chatServer->Start();
	return 0;
}

