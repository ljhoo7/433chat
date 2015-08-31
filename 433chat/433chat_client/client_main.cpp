#include "headers.h"
#include "..\utilities.h"

SOCKET sock;
CReceiver *g_cReceivingManager;

char nickname[STRSIZE] = "";
int room_num;

DWORD WINAPI ReceivingThread(LPVOID arg);

// ���� : ���α׷��� + �����ּ� + ��Ʈ��ȣ + �г��� + ���ȣ
int main(int argc, char *argv[])
{
	int retval;

	// ������ ��ſ� ����� ����
	char buf[STRSIZE], buf2[STRSIZE];
	char *subed;

	room_num = -1;

	printf("[������ ������ ip�ּ�] ", nickname);
	if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
		return 0;
	int wanted = strlen(buf);
	subed = new char[wanted];
	memcpy(subed, buf, wanted);

	unsigned long ip = inet_addr(subed);

	printf("[������ ������ port] ", nickname);
	if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
		return 0;
	wanted = strlen(buf) - 1;
	subed = new char[wanted];
	memcpy(subed, buf, wanted);

	int port = atoi(subed);

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
	serveraddr.sin_addr.s_addr = ip;//SERVERIP);
	serveraddr.sin_port = htons(port);//SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	int len;

	// ������ ������ ���
	while (1){
		// ������ �Է�
		if ((!strcmp("",nickname)) || room_num == -1)
			printf("[��ɾ�] ");
		else
			printf("[%s�� %d�� �濡���� ��ȭ] ", nickname, room_num);
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		bool chat = false;
		std::string tmpstr = buf;
		if (tmpstr.size() > 4)
		{
			std::string subedstr = tmpstr.substr(0, 4);
			if (subedstr == "crea")
			{
				int t_room_num;
				sscanf(tmpstr.c_str(), "create %d\n", &t_room_num);

				if (t_room_num < 0)
				{
					fputs("���ȣ�� 0 �̻��̾�� �˴ϴ�.\n", stdout);
					continue;
				}

				// �� ���� ������
				t_create tmp_packet;
				int size = sizeof(t_create);
				tmp_packet.length = size;
				tmp_packet.type = pkt_type::pt_create;
				tmp_packet.room_num = t_room_num;

				// �� ���� ������ ������
				retval = send(sock, (char*)&tmp_packet, size, 0);
				if (retval == SOCKET_ERROR){
					err_display("send()");
					return 0;
				}
				printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
			}
			else if (subedstr == "join")
			{
				if (room_num == -1)
				{
					sscanf(tmpstr.c_str(), "join %d %s\n", &room_num, buf2);

					if (room_num < 0)
					{
						fputs("���ȣ�� 0 �̻��̾�� �˴ϴ�.\n", stdout);
						continue;
					}

					// '\n' ���� ����
					len = strlen(buf2);

					strcpy(nickname, buf2);

					if (buf2[len - 1] == '\n')
						buf2[len - 1] = '\0';
					if (strlen(buf2) == 0)
						break;

					// �Խ� ������
					t_join tmp_packet;
					int size = len + sizeof(int)+sizeof(short)+sizeof(short);
					tmp_packet.length = size;
					tmp_packet.type = pkt_type::pt_join;
					tmp_packet.room_num = room_num;
					strcpy(tmp_packet.nickname, buf2);

					// �Խ� ������ ������
					retval = send(sock, (char*)&tmp_packet, size, 0);
					if (retval == SOCKET_ERROR){
						err_display("send()");
						break;
					}

					printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
				}
				else
				{
					fputs("��� �� �Խ��� �õ��ϼ���.\n", stdout);
				}
			}
			else if (subedstr == "leav")
			{
				if (room_num == -1)
				{
					fputs("�Խ� �� ����� �õ��ϼ���.\n", stdout);
				}
				else
				{
					sscanf(tmpstr.c_str(), "leave %d %s\n", &room_num, buf2);

					if (room_num < 0)
					{
						fputs("���ȣ�� 0 �̻��̾�� �˴ϴ�.\n", stdout);
						continue;
					}

					// '\n' ���� ����
					len = strlen(buf2);

					strcpy(nickname, "");

					if (buf2[len - 1] == '\n')
						buf2[len - 1] = '\0';
					if (strlen(buf2) == 0)
						break;

					// ��� ������
					t_leave tmp_packet;
					int size = len + sizeof(int)+sizeof(short)+sizeof(short);
					tmp_packet.length = size;
					tmp_packet.type = pkt_type::pt_leave;
					tmp_packet.room_num = room_num;
					strcpy(tmp_packet.nickname, buf2);

					// ��� ������ ������
					retval = send(sock, (char*)&tmp_packet, size, 0);
					if (retval == SOCKET_ERROR){
						err_display("send()");
						break;
					}

					printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

					room_num = -1;
				}
			}
			else
			{
				if (room_num > -1)
				{
					chat = true;
				}
				else
				{
					fputs("�Խ� �� ä���� �õ��ϼ���.\n", stdout);
				}
				
			}
		}
		else
		{
			if (room_num > -1)
			{
				chat = true;
			}
			else
			{
				fputs("�Խ� �� ä���� �õ��ϼ���.\n", stdout);
			}
		}

		if (chat)
		{
			// '\n' ���� ����
			len = strlen(buf);
			if (buf[len - 1] == '\n')
				buf[len - 1] = '\0';
			if (strlen(buf) == 0)
				break;

			t_chat tmp_packet;
			int size = len + sizeof(int)+sizeof(short)+sizeof(short);
			tmp_packet.length = size;
			tmp_packet.type = pkt_type::pt_chat;
			tmp_packet.room_num = room_num;
			strcpy(tmp_packet.str, buf);

			// ä�� ������ ������
			retval = send(sock, (char*)&tmp_packet, size, 0);
			if (retval == SOCKET_ERROR){
				err_display("send()");
				break;
			}
			printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
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