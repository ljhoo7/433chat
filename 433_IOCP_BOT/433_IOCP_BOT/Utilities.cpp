#include "stdafx.h"

// The Customized Printf For Making A Service By This Server Program
bool MyPrintf(PTCH str)
{
#ifdef _DEBUG
	wprintf_s(_T("%s\n"), str);
#else
	
#endif
	return true;
}

// Print Socket Error
void Err_display(PTCH msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	//printf("[%s] %s", msg, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// Decoding the fail signal and Print
void PrintFailSignal(const fail_signal fail)
{
	switch (fail)
	{
	case fail_signal::fs_alreadyexist:
		//std::cout << "Fail : Already exist" << std::endl;
		break;
	case fail_signal::fs_no_exist:
		//std::cout << "Fail : No exist" << std::endl;
		break;
	case fail_signal::fs_overflow:
		//std::cout << "Fail : Overflow" << std::endl;
		break;
	default:
		//std::cout << "Unreadable fail signal." << std::endl;
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