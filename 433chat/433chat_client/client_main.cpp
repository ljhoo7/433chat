#include "headers.h"
#include "..\utilities.h"

SOCKET sock;
CReceiver *g_cReceivingManager;

char *nickname;
int room_num;

DWORD WINAPI ReceivingThread(LPVOID arg);

// 사용법 : 프로그램명 + 서버주소 + 포트번호 + 닉네임 + 방번호
int main(int argc, char *argv[])
{
	int retval;

	if(argc != 5)
	{
		fputs("usage:(program_name) (chat_server_ip) (port) (nick) (room_num)\n",stdout);
		return 0;
	}

	nickname = argv[3];
	room_num = atoi(argv[4]);

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
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);//SERVERIP);
	serveraddr.sin_port = htons(atoi(argv[2]));//SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	char buf[STRSIZE];
	int len;

	// 서버와 데이터 통신
	while (1){
		// 데이터 입력
		printf("['%s'의 보낼 데이터] ", nickname);
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		t_packet tmp_packet;
		tmp_packet.m_chat.type = pkt_type::pt_chat;
		tmp_packet.m_chat.length = sizeof(t_packet);
		strcpy(tmp_packet.m_chat.str, buf);

		// 데이터 보내기
		retval = send(sock, (char*)&tmp_packet, sizeof(t_packet), 0);
		if (retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);
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