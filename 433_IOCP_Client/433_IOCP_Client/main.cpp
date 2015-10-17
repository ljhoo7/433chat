#include "stdafx.h"

mswsock_s g_cMswsock;
CLogWriter *g_pLog;
DWORD g_dwIp;
int t_nPort;

int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	int t_nCPUs = (int)si.dwNumberOfProcessors;

	g_pLog = new CLogWriter(L"ClientLog.log", t_nCPUs);

	if (argc != 3)
	{
		g_pLog->myWprintf(L"The Number of Parameters is error !\n");
	}

	t_nPort = _wtoi(argv[2]);

	if (!(1024 < t_nPort && t_nPort < 66535))
	{
		g_pLog->myWprintf(L"The port number is invalid !\n");
	}

	g_dwIp = _tinet_addr(argv[1]);

	WSADATA wsaData;

	int t_nRc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (t_nRc != 0) {
		printf("WSAStartup failed: %d\n", t_nRc);
		return 1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("Your computer is from the wrong millenium.\n");
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
	}

	t_nRc = closesocket(t_cSock);
	if (t_nRc != 0)
	{
		g_pLog->myWprintf(L"closesocket error on main !");
	}

	CClient t_cClient(t_nCPUs * 2);

	// sleep
	Sleep(INFINITE);

	return 0;
}

