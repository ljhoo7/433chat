#include "utilities.h"

// Print Socket Error and quit
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// Print Socket Error
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// Customized packet receiving function
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0){
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			return SOCKET_ERROR;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

// usage :
//     CHAR msgText[256];
//     getLastErrorText(msgText,sizeof(msgText));
static CHAR*                       //   return error message
getLastErrorText(                  //	converts "Lasr Error" code into text
CHAR *pBuf,                        //   message buffer
ULONG bufSize)                     //   buffer size
{
	DWORD retSize;
	LPTSTR pTemp = NULL;

	if (bufSize < 16) {
		if (bufSize > 0) {
			pBuf[0] = '\0';
		}
		return(pBuf);
	}
	retSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_ARGUMENT_ARRAY,
		NULL,
		GetLastError(),
		LANG_NEUTRAL,
		(LPTSTR)&pTemp,
		0,
		NULL);
	if (!retSize || pTemp == NULL) {
		pBuf[0] = '\0';
	}
	else {
		pTemp[strlen(pTemp) - 2] = '\0'; //remove cr and newline character
		sprintf_s(pBuf, sizeof(pBuf), "%0.*s (0x%x)", bufSize - 16, pTemp, GetLastError());
		LocalFree((HLOCAL)pTemp);
	}
	return(pBuf);
}

void printFailSignal(const fail_signal fail)
{
	switch (fail)
	{
	case fail_signal::fs_alreadyexist:
		std::cout << "Fail : Already exist" << std::endl;
		break;
	case fail_signal::fs_no_exist:
		std::cout << "Fail : No exist" << std::endl;
		break;
	case fail_signal::fs_overflow:
		std::cout << "Fail : Overflow" << std::endl;
		break;
	default:
		std::cout << "Unreadable fail signal." << std::endl;
		break;
	}
}