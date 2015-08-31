#include "headers.h"
#include "..\utilities.h"

SOCKET sock;
CReceiver *g_cReceivingManager;

char nickname[STRSIZE] = "";
int room_num;

DWORD WINAPI ReceivingThread(LPVOID arg);

// 사용법 : 프로그램명 + 서버주소 + 포트번호 + 닉네임 + 방번호
int main(int argc, char *argv[])
{
	int retval;

	// 데이터 통신에 사용할 변수
	char buf[STRSIZE], buf2[STRSIZE];
	char *subed;

	room_num = -1;

	printf("[접속할 서버의 ip주소] ", nickname);
	if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
		return 0;
	int wanted = strlen(buf);
	subed = new char[wanted];
	memcpy(subed, buf, wanted);

	unsigned long ip = inet_addr(subed);

	printf("[접속할 서버의 port] ", nickname);
	if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
		return 0;
	wanted = strlen(buf) - 1;
	subed = new char[wanted];
	memcpy(subed, buf, wanted);

	int port = atoi(subed);

	// 리시빙 매니저 생성
	g_cReceivingManager = new CReceiver();

	// 리시버 스레드 구동
	HANDLE hReceiving = CreateThread(NULL, 0, ReceivingThread, NULL, 0, NULL);

	// 윈속 초기화
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

	// 서버와 데이터 통신
	while (1){
		// 데이터 입력
		if ((!strcmp("",nickname)) || room_num == -1)
			printf("[명령어] ");
		else
			printf("[%s의 %d번 방에서의 대화] ", nickname, room_num);
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
					fputs("방번호는 0 이상이어야 됩니다.\n", stdout);
					continue;
				}

				// 방 생성 데이터
				t_create tmp_packet;
				int size = sizeof(t_create);
				tmp_packet.length = size;
				tmp_packet.type = pkt_type::pt_create;
				tmp_packet.room_num = t_room_num;

				// 방 생성 데이터 보내기
				retval = send(sock, (char*)&tmp_packet, size, 0);
				if (retval == SOCKET_ERROR){
					err_display("send()");
					return 0;
				}
				printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
			}
			else if (subedstr == "join")
			{
				if (room_num == -1)
				{
					sscanf(tmpstr.c_str(), "join %d %s\n", &room_num, buf2);

					if (room_num < 0)
					{
						fputs("방번호는 0 이상이어야 됩니다.\n", stdout);
						continue;
					}

					// '\n' 문자 제거
					len = strlen(buf2);

					strcpy(nickname, buf2);

					if (buf2[len - 1] == '\n')
						buf2[len - 1] = '\0';
					if (strlen(buf2) == 0)
						break;

					// 입실 데이터
					t_join tmp_packet;
					int size = len + sizeof(int)+sizeof(short)+sizeof(short);
					tmp_packet.length = size;
					tmp_packet.type = pkt_type::pt_join;
					tmp_packet.room_num = room_num;
					strcpy(tmp_packet.nickname, buf2);

					// 입실 데이터 보내기
					retval = send(sock, (char*)&tmp_packet, size, 0);
					if (retval == SOCKET_ERROR){
						err_display("send()");
						break;
					}

					printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
				}
				else
				{
					fputs("퇴실 후 입실을 시도하세요.\n", stdout);
				}
			}
			else if (subedstr == "leav")
			{
				if (room_num == -1)
				{
					fputs("입실 후 퇴실을 시도하세요.\n", stdout);
				}
				else
				{
					sscanf(tmpstr.c_str(), "leave %d %s\n", &room_num, buf2);

					if (room_num < 0)
					{
						fputs("방번호는 0 이상이어야 됩니다.\n", stdout);
						continue;
					}

					// '\n' 문자 제거
					len = strlen(buf2);

					strcpy(nickname, "");

					if (buf2[len - 1] == '\n')
						buf2[len - 1] = '\0';
					if (strlen(buf2) == 0)
						break;

					// 퇴실 데이터
					t_leave tmp_packet;
					int size = len + sizeof(int)+sizeof(short)+sizeof(short);
					tmp_packet.length = size;
					tmp_packet.type = pkt_type::pt_leave;
					tmp_packet.room_num = room_num;
					strcpy(tmp_packet.nickname, buf2);

					// 퇴실 데이터 보내기
					retval = send(sock, (char*)&tmp_packet, size, 0);
					if (retval == SOCKET_ERROR){
						err_display("send()");
						break;
					}

					printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

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
					fputs("입실 후 채팅을 시도하세요.\n", stdout);
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
				fputs("입실 후 채팅을 시도하세요.\n", stdout);
			}
		}

		if (chat)
		{
			// '\n' 문자 제거
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

			// 채팅 데이터 보내기
			retval = send(sock, (char*)&tmp_packet, size, 0);
			if (retval == SOCKET_ERROR){
				err_display("send()");
				break;
			}
			printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
		}
	}

	// 리시버 스레드 종료
	CloseHandle(hReceiving);

	// 리시빙 매니저 해지
	delete g_cReceivingManager;

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 1;
}