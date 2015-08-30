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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// connect() : ��� ������ ����õ�
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ip;//SERVERIP);
	serveraddr.sin_port = port;

	if(connect(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)))
	{// ���� ������ listen ���°� �ƴϰų� ���Ͽ����� ��� ���⿡�� ���� ��Ʈ�� ����.
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

		// ��� ������ ����� ����
		SOCKET the_other_sock;
		SOCKADDR_IN theotheraddr;
		int addrlen;

		// ���ŷ �������� �Ͽ��� ��� ������ ������ ������ ����Ѵ�.
		addrlen = sizeof(theotheraddr);
		the_other_sock = accept(listen_sock, (SOCKADDR*)&theotheraddr, &addrlen);
		if(the_other_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			return 0;
		}
	}

	// 3�� �̻� ���� ���� ������ ��ſ� ����� ����
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
