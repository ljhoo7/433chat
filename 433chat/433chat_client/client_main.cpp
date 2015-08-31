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

	room_num = atoi(argv[4]);

	// 룸넘버가 0 ~ ROOM_MAX 사이 구간에 있는 숫자인지 체크해서 아니면 접속 거부
	if(room_num >= ROOM_MAX || room_num < 0)
	{
		printf("A room number must be in this range : 0 ~ %d\n", ROOM_MAX);
		return 0;
	}

	nickname = argv[3];

	// 닉네임 길이 제한
	if(strlen(nickname) > STRSIZE - 1)
	{
		printf("The size of your nickname must be in this range : 1 ~ %d\n", STRSIZE - 1);
		return 0;
	}

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

	// 접속 데이터
	t_join tmp_packet;
	tmp_packet.length = sizeof(t_join) - sizeof(short) - sizeof(short);
	tmp_packet.type = pkt_type::pt_join;
	tmp_packet.room_num = room_num;

	// 접속 데이터 보내기
	retval = send(sock, (char*)&tmp_packet, sizeof(t_join), 0);
	if (retval == SOCKET_ERROR){
		err_display("send()");
		return 0;
	}
	printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

	// 서버와 데이터 통신
	while (1){
		// 데이터 입력
		printf("['%s'의 보낼 데이터] ", nickname);
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
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

		// 채팅 데이터 보내기
		retval = send(sock, (char*)&tmp_packet, len + 20, 0);
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