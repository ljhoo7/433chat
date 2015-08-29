#include "headers.h"
#include "..\utilities.h"

SOCKET sock;
CReceiver *g_cReceivingManager;

DWORD WINAPI ReceivingThread(LPVOID arg);

int main(int argc, char *argv[])
{
	int retval;

	// ���ú� �Ŵ��� ����
	g_cReceivingManager = new CReceiver();

	// ���ù� ������ ����
	HANDLE hReceiving = CreateThread(NULL, 0, ReceivingThread, NULL, 0, NULL);

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ��ſ� ����� ����
	char buf[STRSIZE];
	int len;

	// ������ ������ ���
	while (1){
		// ������ �Է�
		printf("\n[���� ������] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		t_packet tmp_packet;
		tmp_packet.m_chat.type = pkt_type::pt_chat;
		tmp_packet.m_chat.length = sizeof(t_packet);
		strcpy(tmp_packet.m_chat.str, buf);

		// ������ ������
		retval = send(sock, (char*)&tmp_packet, sizeof(t_packet), 0);
		if (retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
	}

	// ���ù� ������ ����
	CloseHandle(hReceiving);

	// ���ú� �Ŵ��� ����
	delete g_cReceivingManager;

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}