#include "headers.h"
#include "..\utilities.h"

#define STR_NUM			6

int block;

char	garbage_strs[STR_NUM][20] = 
{ "Hello World ~ !\0"
, "4:33 intern\0"
, "Random Message\0"
, "Greeting\0"
, "Trash Message\0"
, "I'm a programmer\0" };

int		bot_cnt;
int		room_num;

char	*ip;
char	*port;

HANDLE *hBot;

DWORD WINAPI BotThread(LPVOID arg);
DWORD WINAPI ReceivingThread(LPVOID arg);
DWORD WINAPI BotThread(LPVOID arg);
DWORD WINAPI WaitThread(LPVOID arg);

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
		fputs("usage:(program_name) (chat_server_ip) (port) (bot_cnt) (room_num) (wait_time)\n", stdout);
		return 0;
	}

	room_num = atoi(argv[4]);

	// ��ѹ��� 0 ~ ROOM_MAX ���� ������ �ִ� �������� üũ�ؼ� �ƴϸ� ���� �ź�
	if (room_num >= ROOM_MAX || room_num < 0)
	{
		printf("A room number must be in this range : 0 ~ %d\n", ROOM_MAX);
		return 0;
	}

	bot_cnt = atoi(argv[3]);
	block = atoi(argv[5]);

	ip = argv[1];
	port = argv[2];

	int remain = bot_cnt / MAXIMUM_WAIT_OBJECTS;
	hBot = new HANDLE[bot_cnt];
	HANDLE *hHandleBot = new HANDLE[remain];

	// �� �����
	for (int m = 0; m < bot_cnt; ++m)
	{
		hBot[m] = CreateThread(NULL, 0, BotThread, (LPVOID)&m, 0, NULL);
	}

	for (int m = 0; m < remain; ++m)
	{
		hHandleBot[m] = CreateThread(NULL, 0, WaitThread, (LPVOID)&m, 0, NULL);
	}

	WaitForMultipleObjects(remain, hHandleBot, TRUE, INFINITE);

	for (int m = 0; m < remain; ++m)
	{
		CloseHandle(hHandleBot[m]);
	}

	for(int m = 0; m < bot_cnt; ++m)
	{
		CloseHandle(hBot[m]);
	}

	return 1;
}

DWORD WINAPI WaitThread(LPVOID arg)
{
	int grade = *(int*)arg;
	WaitForMultipleObjects(bot_cnt % MAXIMUM_WAIT_OBJECTS, hBot + (MAXIMUM_WAIT_OBJECTS * grade), TRUE, INFINITE);

	return 0;
}

DWORD WINAPI BotThread(LPVOID arg)
{
	SOCKET sock;
	int m = *(int*)arg;

	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	long long time;
	int retval = 0;
	int size = 0;

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
	serveraddr.sin_addr.s_addr = inet_addr(ip);//SERVERIP);
	serveraddr.sin_port = htons(atoi(port));//SERVERPORT);
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

	// ���ù� ������ ����
	HANDLE hReceiving = CreateThread(NULL, 0, ReceivingThread, (LPVOID)&sock, 0, NULL);

	// ���� ������ ������
	retval = send(sock, (char*)&tmp_packet, sizeof(t_packet), 0);
	if (retval == SOCKET_ERROR){
		err_display("send()");
		return 0;
	}

	while (1)
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time)
		{
			int str_num = rand() % STR_NUM;

			// ���� ������ ����
			//printf("['%s'�� ���� ������] %s", ("bot" + std::to_string(m)).c_str(), garbage_strs[str_num]);

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

			start_time = std::chrono::system_clock::now();
		}
	}

	// ���ù� ������ ����
	CloseHandle(hReceiving);

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
}