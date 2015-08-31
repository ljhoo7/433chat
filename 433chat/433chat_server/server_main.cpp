#include "headers.h"
#include "..\utilities.h"

int nTotalSockets = 0;
SOCKETINFO *SocketInfoArray[FD_SETSIZE];

// 채팅룸
fd_set reads[ROOM_MAX];

// 스레드간 동기화 처리를 위한 임계영역 부가자료
CRITICAL_SECTION cs;

CReceiver		*g_cReceivingManager;

// ip
unsigned long	g_nIp;

// port
int				g_nPort;

// 셀렉트 서버 스레드
DWORD WINAPI ReceivingThread(LPVOID arg);

// 서버간 통신을 위한 스레드
DWORD WINAPI InterServerThread(LPVOID arg);

SOCKET the_other_sock;

int g_nIsListen;

int main(int argc, char *argv[])
{
	int retval;

	if(argc != 4)
	{
		fputs("usage:(program_name) (0=listen,1=connect) (the_other_server_ip) (port)\n",stdout);
		return 0;
	}

	g_nIsListen = atoi(argv[1]);

	if (!(g_nIsListen != 0 || g_nIsListen != 1))
	{
		fputs("The second argument must be zero or one.\n", stdout);
	}

	g_nIp = inet_addr(argv[2]);

	g_nPort = atoi(argv[3]);

	// 리시브 매니저
	g_cReceivingManager = new CReceiver();

	HANDLE hHandle[2];

	hHandle[1] = CreateThread(NULL, 0, InterServerThread, 0, 0, NULL);
	hHandle[0] = CreateThread(NULL, 0, ReceivingThread, 0, 0, NULL);

	WaitForMultipleObjects(1, hHandle, TRUE, INFINITE);

	CloseHandle(hHandle[0]);
	CloseHandle(hHandle[1]);

	// 윈속 종료
	WSACleanup();
	return 0;
}