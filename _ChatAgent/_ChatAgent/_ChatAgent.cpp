#include "stdafx.h"

mswsock_s		g_cMswsock;
CLogWriter		*g_pLog;


DWORD			gMonitoringServerIP;
unsigned int	gMonitoringServerPort;
				
DWORD			gServerIP;
unsigned int	gServerPort;

ServerAgent*	g_pServerAgent;
MServerAgent*   g_pMServerAgent;

int _tmain(int argc, _TCHAR* argv[])
{
	TotalInfo* pTotalInfoData = new TotalInfo;

	bool bExitSignal = false;

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	int t_nCPUs = (int)si.dwNumberOfProcessors;

	g_pLog = new CLogWriter(L"AgentLog.log", t_nCPUs);

	if (argc != 5)
	{
		PRINTF("The Number of Parameters is error !\n");

		exit(0);
	}

	/// server & monitoring server IP & PORT INPUT
	gServerPort = _wtoi(argv[1]);
	gServerIP = _tinet_addr(argv[2]);

	if (!(1024 < gServerPort && gServerPort < 66535))
	{
		PRINTF("The server port number is invalid !\n");

		exit(0);
	}

	gMonitoringServerPort = _wtoi(argv[3]);
	gMonitoringServerIP = _tinet_addr(argv[4]);
	
	if (!(1024 < gMonitoringServerPort && gMonitoringServerPort < 66535))
	{
		PRINTF("The monitoring server port number is invalid !\n");

		exit(0);
	}

	WSADATA wsaData;

	int t_nRc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (t_nRc != 0) {
		PRINTF("WSAStartup failed\n");
		return 1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		PRINTF("Your computer is from the wrong millenium.\n");
		WSACleanup();
		return 1;
	}

	g_pServerAgent = new ServerAgent(pTotalInfoData, t_nCPUs * 2);
	g_pMServerAgent = new MServerAgent(pTotalInfoData, t_nCPUs * 2);

	PRINTF("input command...\n");

	while (!bExitSignal)
	{
		std::string input;
		std::getline(std::cin, input);
		
		if (input == "connect")
		{
			PRINTF("input select connect server type ? ( server / mserver )\n");
			
			std::string connectTarget;
			std::getline(std::cin, connectTarget);

			if (connectTarget == "server")
			{

				g_pServerAgent->Connect(gServerIP, gServerPort);

			}
			else if (connectTarget == "mserver")
			{
				g_pMServerAgent->IsConnected(g_pServerAgent);
				g_pMServerAgent->Connect(gMonitoringServerIP, gMonitoringServerPort);
			
			}
		}
		else if (input == "show")
		{

			PRINTF("\n--------------------------total server info----------------------\n");
			PRINTF("Agent Number : %d \n\n", g_pServerAgent->m_pTotalInfoData->agentNum);
			PRINTF("Room Count : %d \n", g_pServerAgent->m_pTotalInfoData->roomCnt);
			PRINTF("[Room Info List]\n");
			for (auto& room : g_pServerAgent->m_pTotalInfoData->roomInfoList)
			{
				PRINTF("%d\t", room.roomNum);
			}
			PRINTF("\n");
			
			PRINTF("User Count : %d \n", g_pServerAgent->m_pTotalInfoData->userCnt);
			PRINTF("[User Info List]\n");
			for (auto& user : g_pServerAgent->m_pTotalInfoData->userInfoList)
			{
				PRINTF("User Name : %s\n", user.userName);
				PRINTF("User Socket : %d\n", user.userSocket);
				PRINTF("User In RoomNumber : %d\n", user.roomNum);
			}
			PRINTF("\n");

			PRINTF("Server Count : %d\n", g_pServerAgent->m_pTotalInfoData->serverCnt);
			PRINTF("[Server Number List]\n");
			for (auto& serverNum : g_pServerAgent->m_pTotalInfoData->serverNumList)
			{
				PRINTF("%d\t", serverNum);
			}
			PRINTF("\n");

		}
		
	}
	WSACleanup();

	return 0;
}

