#include "stdafx.h"

ChatServer* chatServer;

int _tmain(int argc, _TCHAR* argv[])
{
	
	/*PRINT("%d\n", sizeof(ss_create));
	PRINT("%d\n", sizeof(ss_destroy));
	PRINT("%d\n", sizeof(ss_join));
	PRINT("%d\n", sizeof(ss_leave));
	PRINT("%d\n", sizeof(ss_chat));
	PRINT("%d\n", sizeof(ss_connect));
	PRINT("%d\n", sizeof(ss_disconnect));
	PRINT("%d\n", sizeof(ss_heartbeats));
	PRINT("%d\n", sizeof(ss_heartbeats_response));
	PRINT("%d\n", sizeof(ss_room_info_send));
	PRINT("%d\n", sizeof(ss_player_info_send));
	PRINT("%d\n", sizeof(ss_room_info_success));
	PRINT("%d\n", sizeof(ss_room_info_failure));
	PRINT("%d\n", sizeof(ss_player_info_success));
	PRINT("%d\n", sizeof(ss_player_info_failure));
	PRINT("%d\n", sizeof(ss_sync_req));*/

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

