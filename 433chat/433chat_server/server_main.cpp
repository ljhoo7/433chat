#include "headers.h"
#include "..\utilities.h"

int nTotalSockets = 0;
SOCKETINFO *SocketInfoArray[FD_SETSIZE];

// ä�÷�
fd_set reads[ROOM_MAX];

// �����尣 ����ȭ ó���� ���� �Ӱ迵�� �ΰ��ڷ�
CRITICAL_SECTION cs;

CReceiver		*g_cReceivingManager;

// ip
unsigned long	g_nIp;

// port
int				g_nPort;

// ����Ʈ ���� ������
DWORD WINAPI ReceivingThread(LPVOID arg);

// ������ ����� ���� ������
DWORD WINAPI InterServerThread(LPVOID arg);

int main(int argc, char *argv[])
{
	int retval;

	if(argc != 3)
	{
		fputs("usage:(program_name) (the_other_server_ip) (port)\n",stdout);
		return 0;
	}

	g_nIp = inet_addr(argv[1]);

	g_nPort = atoi(argv[2]);

	// ���ú� �Ŵ���
	g_cReceivingManager = new CReceiver();

	HANDLE hHandle[2];
	hHandle[0] = CreateThread(NULL, 0, ReceivingThread, 0, 0, NULL);
	hHandle[1] = CreateThread(NULL, 0, InterServerThread, 0, 0, NULL);

	WaitForMultipleObjects(2, hHandle, TRUE, INFINITE);

	CloseHandle(hHandle[0]);
	CloseHandle(hHandle[1]);

	// ���� ����
	WSACleanup();
	return 0;
}