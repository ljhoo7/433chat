#include "headers.h"
#include "..\utilities.h"

#define STR_NUM			6

int block;

SOCKET sock;
CReceiver *g_cReceivingManager;

char	garbage_strs[STR_NUM][20] = 
{ "Hello World ~ !\0"
, "4:33 intern\0"
, "Random Message\0"
, "Greeting\0"
, "Trash Message\0"
, "I'm a programmer\0" };

char	*bot_name;
int		room_num;

DWORD WINAPI ReceivingThread(LPVOID arg);

// ���� : ���α׷��� + �����ּ� + ��Ʈ��ȣ + �� �̸� + ���ȣ + ���ð�
int main(int argc, char *argv[])
{
	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	long long time;
	int retval = 0;
	int size = 0;

	if (argc != 6)
	{
		fputs("usage:(program_name) (chat_server_ip) (port) (bot_name) (room_num) (wait_time)\n", stdout);
		return 0;
	}

	room_num = atoi(argv[4]);

	// ��ѹ��� 0 ~ ROOM_MAX ���� ������ �ִ� �������� üũ�ؼ� �ƴϸ� ���� �ź�
	if (room_num >= ROOM_MAX || room_num < 0)
	{
		printf("A room number must be in this range : 0 ~ %d\n", ROOM_MAX);
		return 0;
	}

	bot_name = argv[3];

	// �г��� ���� ����
	if (strlen(bot_name) > STRSIZE - 1)
	{
		printf("The size of your bot name must be in this range : 1 ~ %d\n", STRSIZE - 1);
		return 0;
	}

	block = atoi(argv[5]);

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
	t_packet tmp_packet;
	tmp_packet.m_join.type = pkt_type::pt_join;
	tmp_packet.m_join.length = sizeof(t_packet);
	tmp_packet.m_join.room_num = room_num;

	// ���� ������ ������
	retval = send(sock, (char*)&tmp_packet, sizeof(t_packet), 0);
	if (retval == SOCKET_ERROR){
		err_display("send()");
		return 0;
	}
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

	// ������ ������ ���
	while (1){
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){
			// ������ �Է�
			/*printf("['%s'�� ���� ������] ", bot_name);
			if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
				break;*/
			int str_num = rand() % STR_NUM;

			if (strlen(buf) == 0)
				break;

			t_packet tmp_packet;
			tmp_packet.m_chat.type = pkt_type::pt_chat;
			tmp_packet.m_chat.length = sizeof(t_packet);
			tmp_packet.m_chat.room_num = room_num;
			strcpy(tmp_packet.m_chat.str, garbage_strs[str_num]);

			// ä�� ������ ������
			retval = send(sock, (char*)&tmp_packet, sizeof(t_packet), 0);
			if (retval == SOCKET_ERROR){
				err_display("send()");
				break;
			}
			printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

			start_time = std::chrono::system_clock::now();
		}
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