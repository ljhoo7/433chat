#include "stdafx.h"

extern CLogWriter *g_pLog;

// Decoding the fail signal and Print
void PrintFailSignal(const fail_signal fail)
{
	switch (fail)
	{
	case fail_signal::fs_alreadyexist:
		g_pLog->myWprintf(L"Fail : Already exist");
		break;
	case fail_signal::fs_no_exist:
		g_pLog->myWprintf(L"Fail : No exist");
		break;
	case fail_signal::fs_overflow:
		g_pLog->myWprintf(L"Fail : Overflow");
		break;
	default:
		g_pLog->myWprintf(L"Unreadable fail signal.");
		break;
	}
}

DWORD _tinet_addr(const TCHAR *cp)
{
#ifdef UNICODE
	char IP[16];
	int t_nRet = 0;
	t_nRet = WideCharToMultiByte(CP_ACP, 0, cp, _tcslen(cp), IP, 15, NULL, NULL);
	IP[t_nRet] = 0;
	return inet_addr(IP);
#endif
#ifndef UNICODE
	return init_addr(cp);
#endif
}

void BeforeMakingIOCPMessage(PTCH p_szStr)
{
#ifdef _DEBUG
	if (0 > wprintf_s(p_szStr))
		wprintf_s(L"The OS has failed to write a message in debug mode before making IOCP.\n");
	system("pause");
	exit(1);
#else
	// If this state is the release mode, then make the beep sound thrice !!!
	if (0 > wprintf_s(L"\a\a\a"))
		wprintf_s(L"\a\a\a\a");
	system("pause");
	exit(1);
	// Kill This Service ! Don't use KillProcess or TerminateProcess !
#endif
}