#include "stdafx.h"

CProactor					*g_pProactor;
CLogWriter					*g_pLog;

// The Entry Point Function
int _tmain(int argc, _TCHAR* argv[])
{
	int t_nRetval;

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	int t_nCPUs = (int)si.dwNumberOfProcessors;

	g_pLog = new CLogWriter("BotLog.log", 2);

	if (6 != argc)
	{
		g_pLog->myWprintf("(usage error) usage : (program_name) (server_ip) (server_port) (bot_cnt) (chat_interval_min) (chat_interval_max)\n");

		return 1;
	}

	DWORD		t_dwIp			= inet_addr(argv[1]);
	int			t_nPort			= atoi(argv[2]);
	int			t_nBotCnt		= atoi(argv[3]);
	int			t_nIntervalMin	= atoi(argv[4]);
	int			t_nIntervalMax  = atoi(argv[5]);


	if (TOTAL_PLAYER < t_nBotCnt)
	{
		g_pLog->myWprintf("The Number of Bots is so big !!!\n");
		return 1;
	}

	WSADATA wsaData;

	t_nRetval = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (t_nRetval)
	{
		g_pLog->myWprintf("WSAStartup failed\n");
		return 1;
	}

	if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))
	{
		g_pLog->myWprintf("Your computer is from the wrong millenium.\n");
		WSACleanup();
		return 1;
	}

	g_pProactor = new CProactor(t_dwIp, t_nPort, t_nBotCnt, t_nCPUs << 1, t_nIntervalMin, t_nIntervalMax);
	g_pProactor->Init();

	WaitForMultipleObjects(t_nBotCnt, g_pProactor->m_hEventForAllJoin, TRUE, INFINITE);

	g_pLog->myWprintf("all done !!!\n");

	for (std::vector<CBot*>::iterator iter = g_pProactor->m_vBot.begin(); iter != g_pProactor->m_vBot.end()
		; ++iter)
	{
		(*iter)->BeginChatThread();
	}

	//g_pProactor->m_vBot[99]->SendChatMessage("hihi");

	Sleep(INFINITE);

	WSACleanup();

	return 0;
}