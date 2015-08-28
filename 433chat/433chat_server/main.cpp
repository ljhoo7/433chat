// WindowsSelectServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "../skel.h"

void error_handling(const char *buf);
void paket_handling(t_packet pkt, int i, fd_set reads);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET serv_sock, clnt_sock;
	SOCKADDR_IN serv_adr, clnt_adr;
	TIMEVAL timeout;
	fd_set reads, cpy_reads;
	t_packet buf;

	int adr_sz;
	int str_len, fd_num, i, m;

	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		error_handling("WSAStartup() error!");

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == SOCKET_ERROR)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);

	while (1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fd_num = select(0, &cpy_reads, 0, 0, &timeout)) == SOCKET_ERROR)
			break;
		if (fd_num == 0)
			continue;
		for (i = 0; i < reads.fd_count; i++)
		{
			int sockNum = reads.fd_array[i];
			if (FD_ISSET(sockNum, &cpy_reads))
			{
				if (sockNum == serv_sock)
				{
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					printf("connected client: %d\n", i);
				}
				else
				{
					str_len = recv(sockNum, (char*)&buf, sizeof(t_packet), 0);
					if (str_len == 0)
					{
						FD_CLR(sockNum, &reads);
						closesocket(sockNum);
						printf("closed client:%d\n", i);
					}
					else
					{
						paket_handling(buf, i, reads);
					}
				}
			}
		}
	}

	return 0;
}

void paket_handling(t_packet pkt, int i, fd_set reads)
{
	int m;
	t_packet result_pkt;
	switch (pkt.m_any.type)
	{
	case pt_move:
		printf("move : (%f, %f, %f)=>(%f, %f, %f)\n", pkt.m_move.start_coord.x, pkt.m_move.start_coord.y, pkt.m_move.start_coord.z, pkt.m_move.dest_coord.x, pkt.m_move.dest_coord.y, pkt.m_move.dest_coord.z);

		pkt.m_move.time = 0;

		for (m = 0; m < reads.fd_count; ++m)
		{
			if (m != i)
			{
				printf("%d's moving message was sent to %d\n", i, m);
				send(reads.fd_array[m], (char*)&pkt, sizeof(t_packet), 0);
			}
		}
		break;
	}
}

void error_handling(const char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}