#include "headers.h"
#include "..\utilities.h"

extern int fps;
extern double block;

extern unsigned int ip;
extern int port;

DWORD WINAPI InterServerThread(LPVOID arg)
{
	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;
	
	long long time;
	int retval = 0;
	int size = 0;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// connect() : 상대 서버에 연결시도
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ip;//SERVERIP);
	serveraddr.sin_port = port;

	if(connect(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)))
	{// 상대방 서버가 listen 상태가 아니거나 소켓에러일 경우 여기에서 연결 포트를 연다.
		// bind()
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons(SERVERPORT);
		retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
		if(retval == SOCKET_ERROR) err_quit("bind()");

		// listen()
		retval = listen(listen_sock, SOMAXCONN);
		if(retval == SOCKET_ERROR) err_quit("listen()");

		// 상대 서버와 통신할 변수
		SOCKET the_other_sock;
		SOCKADDR_IN theotheraddr;
		int addrlen;

		// 블로킹 소켓으로 하여서 상대 서버가 접속할 때까지 대기한다.
		addrlen = sizeof(theotheraddr);
		the_other_sock = accept(listen_sock, (SOCKADDR*)&theotheraddr, &addrlen);
		if(the_other_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			return 0;
		}
	}

	// 3개 이상 서버 간의 데이터 통신에 사용할 변수
	/*FD_SET socks, cpy_socks;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, str_len;
	t_packet buf;

	FD_ZERO(&socks);
	FD_SET(listen_sock, &socks);*/

	while(1)		
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){

			start_time = std::chrono::system_clock::now();
		}
	}
	return true;
}

CInterServer::CInterServer(void)
{
}


CInterServer::~CInterServer(void)
{
}
