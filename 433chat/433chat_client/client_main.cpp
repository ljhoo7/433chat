#include "headers.h"
#include "..\utilities.h"

SOCKET sock;
CReceiver *g_cReceivingManager;

char *nickname;
int room_num;

DWORD WINAPI ReceivingThread(LPVOID arg);

// ���� : ���α׷��� + �����ּ� + ��Ʈ��ȣ + �г��� + ���ȣ
int main(int argc, char *argv[])
{
	int retval;

	if(argc != 5)
	{
		fputs("usage:(program_name) (chat_server_ip) (port) (nick) (room_num)\n",stdout);
		return 0;
	}

	room_num = atoi(argv[4]);

	// ��ѹ��� 0 ~ ROOM_MAX ���� ������ �ִ� �������� üũ�ؼ� �ƴϸ� ���� �ź�
	if(room_num >= ROOM_MAX || room_num < 0)
	{
		printf("A room number must be in this range : 0 ~ %d\n", ROOM_MAX);
		return 0;
	}

	nickname = argv[3];

	// �г��� ���� ����
	if(strlen(nickname) > STRSIZE - 1)
	{
		printf("The size of your nickname must be in this range : 1 ~ %d\n", STRSIZE - 1);
		return 0;
	}

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
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);//SERVERIP);
	serveraddr.sin_port = htons(atoi(argv[2]));//SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ��ſ� ����� ����
	char buf[STRSIZE];
	int len;

	// ���� ������
	t_join tmp_packet;
	tmp_packet.length = sizeof(t_join) - sizeof(short) - sizeof(short);
	tmp_packet.type = pkt_type::pt_join;
	tmp_packet.room_num = room_num;

	// ���� ������ ������
	retval = send(sock, (char*)&tmp_packet, sizeof(t_join), 0);
	if (retval == SOCKET_ERROR){
		err_display("send()");
		return 0;
	}
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

	// ������ ������ ���
	while (1){
		// ������ �Է�
		printf("['%s'�� ���� ������] ", nickname);
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf);
		//std::cout << len << std::endl;
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		t_chat tmp_packet;
		tmp_packet.length = len + sizeof(int);
		tmp_packet.type = pkt_type::pt_chat;
		tmp_packet.room_num = room_num;
		strcpy(tmp_packet.str, buf);

		// ä�� ������ ������
		retval = send(sock, (char*)&tmp_packet, len + 20, 0);
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
	return 1;
}