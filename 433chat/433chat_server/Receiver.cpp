#include "headers.h"
#include "..\utilities.h"

int fps = 30;
double block = 1000 / fps;

extern CReceiver		*g_cReceivingManager;

// ä�÷�
extern fd_set			reads[ROOM_MAX];

extern SOCKET the_other_sock;

extern unsigned long g_nIp;
extern int g_nPort;

extern int g_nIsListen;

// ��Ŷ ó�� �Լ�
void paket_handling(t_packet pkt, int i, SOCKET sock);

// �޼��� �Ѹ��� ������ �Լ�
DWORD WINAPI SpreadingThread(LPVOID arg);

// ����Ʈ ���� ������
DWORD WINAPI ReceivingThread(LPVOID arg)
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

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (g_nIsListen)
		serveraddr.sin_port = htons(SERVERPORT1);
	else
		serveraddr.sin_port = htons(SERVERPORT2);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// �ͺ��ŷ �������� ��ȯ
	/*u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if(retval == SOCKET_ERROR) err_display("ioctlsocket()");*/

	// ������ ��ſ� ����� ����
	FD_SET socks, cpy_socks;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, str_len;
	t_packet buf;

	FD_ZERO(&socks);
	FD_SET(listen_sock, &socks);

	while(1)		
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){
			cpy_socks = socks;

			// select()
			retval = select(0, &cpy_socks, 0, NULL, NULL);
			if(retval == SOCKET_ERROR)
				err_quit("select()");
			if(retval == 0)
				continue;
			for(int i = 0; i < socks.fd_count; i++)
			{
				int sockNum = socks.fd_array[i];
				if(FD_ISSET(sockNum, &cpy_socks))
				{
					if(sockNum == listen_sock)
					{
						addrlen = sizeof(clientaddr);
						client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
						FD_SET(client_sock, &socks);
						printf("connected client: %d, cnt : %d\n", client_sock, socks.fd_count);
					}
					else
					{
						str_len = recvn(sockNum, (char*)&buf, sizeof(t_packet), 0);
						if(str_len == SOCKET_ERROR)
						{
							FD_CLR(sockNum, &socks);
							closesocket(sockNum);
							printf("closed client:%d, cnt : %d\n", socks.fd_array[i], socks.fd_count);
						}
						else
						{
							paket_handling(buf, i, sockNum);
						}
					}	
				}
			}

			start_time = std::chrono::system_clock::now();
		}
	}
	return true;
}

CReceiver::CReceiver(void)
{
}


CReceiver::~CReceiver(void)
{
}

// ��Ŷ ó�� �Լ�
void paket_handling(t_packet pkt, int i, SOCKET sock)
{
	int nRoom, retval;
	t_packet result_pkt;
	thread_data tData;
	HANDLE hSpreading;
	switch(pkt.m_any.type)
	{
		case pt_chat:
			nRoom = pkt.m_chat.room_num;

			tData.room_num = nRoom;
			tData.pkt = pkt;
			tData.sock = sock;

			// ��Ƽ ������� ó���� �κ� : ���� �� �ο��鿡�� �޼��� �Ѹ���
			//InitializeCriticalSection(&cs);
			hSpreading = CreateThread(NULL, 0, SpreadingThread,
				(LPVOID)&tData, 0, NULL);

			CloseHandle(hSpreading);
			
			//DeleteCriticalSection(&cs);
			
			// ��� ������ ������ �Ǿ��ִٸ� ê�� �޼����� �����ش�.
			if (the_other_sock != NULL)
				send(the_other_sock, (char*)&pkt, sizeof(t_packet), 0);
			
			break;
		case pt_join:
			// ��Ƽ ������� ó���� �κ� : �����Ϸ��� �濡 ���� �г����� ����� �ִ���
			/*for(m = 0; m < reads[nRoom].fd_count; ++m)
			{
				if( reads[nRoom].fd_array[m] != sock )
				{
					
				}
			}*/
			
			//EnterCriticalSection(&cs);

			nRoom = pkt.m_join.room_num;
			FD_SET(sock, &reads[nRoom]);

			//LeaveCriticalSection(&cs);
			break;
	}
}

// �޼��� �Ѹ��� �Լ�
DWORD WINAPI SpreadingThread(LPVOID arg)
{
	thread_data *tData = (thread_data*)arg;
	
	for(int m = 0; m < reads[tData->room_num].fd_count; ++m)
	{
		if( reads[tData->room_num].fd_array[m] != tData->sock )
		{
			send(reads[tData->room_num].fd_array[m], (char*)&tData->pkt, sizeof(t_packet), 0);
		}
	}

	return 0;
}