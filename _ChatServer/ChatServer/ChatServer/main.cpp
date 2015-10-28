#include "stdafx.h"

ChatServer* chatServer;

int _tmain(int argc, _TCHAR* argv[])
{
	
	/*printf("%d\n", sizeof(ss_create));
	printf("%d\n", sizeof(ss_destroy));
	printf("%d\n", sizeof(ss_join));
	printf("%d\n", sizeof(ss_leave));
	printf("%d\n", sizeof(ss_chat));
	printf("%d\n", sizeof(ss_connect));
	printf("%d\n", sizeof(ss_disconnect));
	printf("%d\n", sizeof(ss_heartbeats));
	printf("%d\n", sizeof(ss_heartbeats_response));
	printf("%d\n", sizeof(ss_room_info_send));
	printf("%d\n", sizeof(ss_player_info_send));
	printf("%d\n", sizeof(ss_room_info_success));
	printf("%d\n", sizeof(ss_room_info_failure));
	printf("%d\n", sizeof(ss_player_info_success));
	printf("%d\n", sizeof(ss_player_info_failure));
	printf("%d\n", sizeof(ss_sync_req));*/

	if (argc != 3){
		PRINTF("error command\n");
		return 0;
	}

	WORD agentPORT = atoi(argv[2]);
	WORD serverNum = atoi(argv[1]);
	chatServer = new ChatServer(serverNum, agentPORT);

	chatServer->Start();
	return 0;
}

