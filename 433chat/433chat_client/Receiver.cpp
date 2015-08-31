#include "headers.h"
#include "..\utilities.h"

int fps = 30;
double block = 1000 / fps;

extern SOCKET			sock;
extern CReceiver		*g_cReceivingManager;

extern char				*nickname;
extern int				room_num;

DWORD WINAPI ReceivingThread(LPVOID arg)
{
	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;
	
	long long time;
	int retval = 0;
	int size = 0, sum = 0;

	t_join tmpJoin;
	t_chat tmpChat;

	while(1)		
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){

			sum = 0;

			retval = 0;

			short length, type;

			retval = recvn(sock, (char*)&length, sizeof(short), 0);
			if(retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error");
			sum += retval;

			retval = recvn(sock, (char*)&type, sizeof(short), 0);
			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error");
			sum += retval;

			char *datum = new char[length];

			retval = recvn(sock, (char*)&datum, length, 0);
			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error");
			sum += retval;

			switch ((pkt_type)type)
			{
			case pkt_type::pt_chat:
				ZeroMemory(&tmpChat, sizeof(t_chat));
				memcpy((&tmpChat) + 4, datum, length);

				printf("\n[TCP 클라이언트] %d바이트를 받았습니다.\n", sum);
				printf("[받은 데이터] %s\n", tmpChat.str);
				printf("['%s'의 보낼 데이터] ", nickname);
				break;
			case pkt_type::pt_join:
				ZeroMemory(&tmpJoin, sizeof(t_join));
				memcpy((&tmpJoin) + 4, datum, length);
				break;
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
