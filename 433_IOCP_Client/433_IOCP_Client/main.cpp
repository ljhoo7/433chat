#include "stdafx.h"

mswsock_s		g_cMswsock;
CLogWriter		*g_pLog;
DWORD			g_dwIp;
int				t_nPort;

CClient			*g_pClient;

bool			g_bExitSignal;

int _tmain(int argc, _TCHAR* argv[])
{
	g_bExitSignal = false;

	SYSTEM_INFO si;
	memset((void*)&si, 0, sizeof(SYSTEM_INFO));
	GetSystemInfo(&si);

	int t_nCPUs = (int)si.dwNumberOfProcessors;

	if (0 == t_nCPUs)
	{
		g_pLog->myWprintf(L"error ! t_nCPUs is zero in main !!!\n");
		return 1;
	}

	g_pLog = new CLogWriter(L"ClientLog.log", t_nCPUs);

	if (NULL == g_pLog)
	{
		g_pLog->myWprintf(L"g_pLog is NULL in main !\n");
		return 1;
	}

	if (argc != 3)
	{
		g_pLog->myWprintf(L"The Number of Parameters is error !\n");

		return 1;
	}

	t_nPort = _wtoi(argv[2]);

	if (!(1024 < t_nPort && t_nPort < 66535))
	{
		g_pLog->myWprintf(L"The port number is invalid !\n");

		return 1;
	}

	g_dwIp = _tinet_addr(argv[1]);

	if (INADDR_NONE == g_dwIp)
	{
		g_pLog->myWprintf(L"g_dwIp is invalid !!\n");

		return 1;
	}

	WSADATA wsaData;

	int t_nRc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (t_nRc != 0) {
		g_pLog->myWprintf(L"WSAStartup failed\n");
		return 1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		g_pLog->myWprintf(L"Your computer is from the wrong millenium.\n");
		WSACleanup();
		return 1;
	}

	SOCKET t_cSock;
	DWORD t_dwBytes;

	/* Dummy socket needed for WSAIoctl */
	t_cSock = socket(AF_INET, SOCK_STREAM, 0);
	if (t_cSock == INVALID_SOCKET)
		return FALSE;

	GUID t_cGuid = WSAID_CONNECTEX;
	t_nRc = WSAIoctl(t_cSock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&t_cGuid, sizeof(t_cGuid),
		&g_cMswsock.ConnectEx, sizeof(g_cMswsock.ConnectEx),
		&t_dwBytes, NULL, NULL);

	if (t_nRc != 0)
	{
		g_pLog->myWprintf(L"WSAloctl error on main !");

		exit(0);
	}

	t_nRc = closesocket(t_cSock);
	if (t_nRc != 0)
	{
		g_pLog->myWprintf(L"closesocket error on main !");

		exit(0);
	}

	g_pClient = new CClient(t_nCPUs * 2);

	if (NULL == g_pClient)
	{
		g_pLog->myWprintf(L"g_pClient is NULL in main !\n");
		return 1;
	}

	bool t_bRetval = g_pClient->Init();
	if (!t_bRetval)
	{
		g_pLog->myWprintf(L"g_pClient->Init return value is false in main !\n");
		return 1;
	}

	std::string tmpStr, subStr;
	int t_nTmpRoomNum, t_nRetVal;
	char t_szTmpNick[NICK_SIZE];

	bool t_bIsLobby = false, t_bIsRoom = false;

	while (!g_bExitSignal)
	{
		std::getline(std::cin, tmpStr);
		if (4 <= tmpStr.size())
		{
			// check the state of the client
			if (CLobby::Instance() == g_pClient->GetStateMachine()->CurrentState())
			{
				t_bIsLobby = true;
				t_bIsRoom = false;
			}
			else if (CRoom::Instance() == g_pClient->GetStateMachine()->CurrentState())
			{
				t_bIsLobby = false;
				t_bIsRoom = true;
			}
			else
			{
				t_bIsLobby = false;
				t_bIsRoom = false;
			}

			subStr = tmpStr.substr(0, 4);
			if ("crea" == subStr)
			{
				if (t_bIsLobby)
				{
					t_nRetVal = sscanf(tmpStr.c_str(), "create %d\n", &t_nTmpRoomNum);
					if (0 >= t_nRetVal)
					{
						g_pLog->myWprintf(L"You must try to chat when you are in a room !\n");
					}
					else
					{
						g_pClient->SendCreateMessage(t_nTmpRoomNum);
					}
				}
				else
				{
					g_pLog->myWprintf(L"You must try to create when you are in a lobby !\n");
				}
			}
			else if ("dest" == subStr)
			{
				if (t_bIsLobby)
				{
					t_nRetVal = sscanf(tmpStr.c_str(), "destroy %d\n", &t_nTmpRoomNum);
					if (0 >= t_nRetVal)
					{
						g_pLog->myWprintf(L"You must try to chat when you are in a room !\n");
					}
					else
					{
						g_pClient->SendDestroyMessage(t_nTmpRoomNum);
					}
				}
				else
				{
					g_pLog->myWprintf(L"You must try to destroy when you are in a lobby !\n");
				}
			}
			else if ("join" == subStr)
			{
				if (t_bIsLobby)
				{
					t_nRetVal = sscanf(tmpStr.c_str(), "join %d %s\n", &t_nTmpRoomNum, t_szTmpNick);
					if (2 != t_nRetVal)
					{
						g_pLog->myWprintf(L"You must try to chat when you are in a room !\n");
					}
					else
					{
						g_pClient->SendJoinMessage(t_nTmpRoomNum, t_szTmpNick);
					}
				}
				else
				{
					// chat
					g_pLog->myWprintf(L"You must try to chat when you are in a room !\n");
				}
			}
			else if ("leav" == subStr)
			{
				if (t_bIsRoom)
				{
					if ("leave" == tmpStr)
					{
						g_pClient->SendLeaveMessage();
					}
					else
					{
						// chat
						g_pClient->SendChatMessage(tmpStr);
					}
				}
			}
			else
			{
				if (t_bIsRoom)
				{
					// chat
					g_pClient->SendChatMessage(tmpStr);
				}
				else
				{
					if ("exit" == tmpStr)
					{
						g_bExitSignal = true;
					}
					else
						g_pLog->myWprintf(L"You must try to chat when you are in a room !\n");
				}
			}
		}
		else
		{
			// chat
			g_pClient->SendChatMessage(tmpStr);
		}
	}

	return 0;
}

