#include "headers.h"
#include "..\utilities.h"

int nTotalSockets = 0;
SOCKETINFO *SocketInfoArray[FD_SETSIZE];

// ���� ���� �Լ�
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

// ��Ŷ ó�� �Լ�
void paket_handling(t_packet pkt, int i, fd_set reads);

int main(int argc, char *argv[])
{
	int retval;

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
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// �ͺ��ŷ �������� ��ȯ
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if(retval == SOCKET_ERROR) err_display("ioctlsocket()");

	// ������ ��ſ� ����� ����
	FD_SET socks, cpy_socks;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, str_len;
	t_packet buf;

	FD_ZERO(&socks);
	FD_SET(listen_sock, &socks);

	while(1){
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
					printf("connected client: %d\n", client_sock);
				}
				else
				{
					str_len = recvn(sockNum, (char*)&buf, sizeof(t_packet), 0);
					if(str_len == SOCKET_ERROR)
					{
						FD_CLR(sockNum, &socks);
						closesocket(sockNum);
						printf("closed client:%d\n", socks.fd_array[i]);
					}
					else
					{
						paket_handling(buf, i, socks);
					}
				}	
			}
		} 	
	}

	// ���� ����
	WSACleanup();
	return 0;
}

// ��Ŷ ó�� �Լ�
void paket_handling(t_packet pkt, int i, fd_set reads)
{
	int m;
	t_packet result_pkt;
	switch(pkt.m_any.type)
	{
		case pt_chat:
			for(m = 0; m < reads.fd_count; ++m)
			{
				if( m != i )
				{
					send(reads.fd_array[m], (char*)&pkt, sizeof(t_packet), 0);
				}
			}
			break;
	}
}

// ���� ���� �߰�
BOOL AddSocketInfo(SOCKET sock)
{
	if(nTotalSockets >= FD_SETSIZE){
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return FALSE;
	}

	SOCKETINFO *ptr = new SOCKETINFO;
	if(ptr == NULL){
		printf("[����] �޸𸮰� �����մϴ�!\n");
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	SocketInfoArray[nTotalSockets++] = ptr;

	return TRUE;
}

// ���� ���� ����
void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO *ptr = SocketInfoArray[nIndex];

	// Ŭ���̾�Ʈ ���� ���
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", 
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	closesocket(ptr->sock);
	delete ptr;

	if(nIndex != (nTotalSockets-1))
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets-1];

	--nTotalSockets;
}