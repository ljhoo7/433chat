#include "utilities.h"
#include "RoomManager.h"

int fps = 30;
double block = 1000 / fps;

SOCKET the_other_sock;
extern int g_nIsListen;

extern RoomManager roomManager;

DWORD WINAPI InterServerThread(LPVOID arg)
{
	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	long long time;
	int retval = 0;
	int size = 0;

	the_other_sock = NULL;

	{// ���� ������ listen ���°� �ƴϰų� ���Ͽ����� ��� ���⿡�� ���� ��Ʈ�� ����.
		// ���� �ʱ�ȭ

		if (g_nIsListen)
		{
			WSADATA wsa;
			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
				return 1;

			printf("This server is attempting to connect...\n");

			// socket()
			the_other_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (the_other_sock == INVALID_SOCKET) err_quit("socket()");

			// connect() : ��� ������ ����õ�
			SOCKADDR_IN serveraddr;
			ZeroMemory(&serveraddr, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
			serveraddr.sin_port = htons(INTERSERVERPORT);

			retval = connect(the_other_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
			if (retval == SOCKET_ERROR)err_quit("connect()");

			printf("This server has been connected to the other server by connect().\n");
		}
		else
		{
			WSADATA wsa;
			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
				return 1;

			printf("This server will be waiting for The other server.\n");

			// socket()
			SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (listen_sock == INVALID_SOCKET) err_quit("socket()");

			// bind()
			SOCKADDR_IN serveraddr;
			ZeroMemory(&serveraddr, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
			serveraddr.sin_port = htons(INTERSERVERPORT);
			retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

			// listen()
			retval = listen(listen_sock, SOMAXCONN);
			if (retval == SOCKET_ERROR) err_quit("listen()");

			// ��� ������ ����� ����
			SOCKADDR_IN theotheraddr;
			int addrlen;

			// ���ŷ �������� �Ͽ��� ��� ������ ������ ������ ����Ѵ�.
			addrlen = sizeof(theotheraddr);
			the_other_sock = accept(listen_sock, (SOCKADDR*)&theotheraddr, &addrlen);
			if (the_other_sock == INVALID_SOCKET)
			{
				err_display("accept()");
				return 0;
			}
			printf("This server has been connected to the other server by accept().\n");
		}
	}

	printf("InterServer Thread has been activated.\n");

	// 3�� �̻� ���� ���� ������ ��ſ� ����� ����
	/*FD_SET socks, cpy_socks;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, str_len;
	t_packet buf;

	FD_ZERO(&socks);
	FD_SET(listen_sock, &socks);*/

	while (1)
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){
			char buf[PKTLEN];
			short length;
			int str_len = recvn(the_other_sock, (char*)&length, sizeof(short), 0);

			printf("%d\n", str_len);

			if (str_len == SOCKET_ERROR)
			{
				closesocket(the_other_sock);
				printf("closed the other server.\n");
				return true;
			}

			printf("size:%d\n", length);

			int remain = length - 2;
			str_len = recvn(the_other_sock, (char*)buf, remain, 0);

			printf("%d\n", str_len);

			if (str_len == SOCKET_ERROR)
			{
				closesocket(the_other_sock);
				printf("closed the other server.\n");
				return true;
			}

			short type;
			memcpy(&type, buf, sizeof(short));

			switch (type)
			{
			case pkt_type::pt_chat:
				t_chat tmpChat;
				tmpChat.length = length;
				memcpy(&tmpChat.type, buf, remain);
				roomManager.findRoom(tmpChat.room_num)->broadcast_msg((char*)&tmpChat, tmpChat.length);
				break;
			case pkt_type::pt_create:
				t_create tmpCreate;
				tmpChat.length = length;
				memcpy(&tmpCreate.type, buf, remain);
				roomManager.createRoom(tmpCreate.room_num);
				break;
			}

			start_time = std::chrono::system_clock::now();
		}
	}
	return true;
}
