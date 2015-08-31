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

	t_join	tmpJoin;
	t_chat	tmpChat;
	t_leave tmpLeave;

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

			std::cout << length << std::endl;

			int remain = length - sizeof(short);

			char *datum = new char[remain];

			retval = recvn(sock, (char*)&datum, remain, 0);
			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error");
			sum += retval;

			memcpy(&type, datum, sizeof(short));

			switch ((pkt_type)type)
			{
			case pkt_type::pt_chat:
				ZeroMemory(&tmpChat, sizeof(t_chat));
				memcpy((&tmpChat) + sizeof(short), datum, remain);

				printf("\n[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", sum);
				printf("[���� ������] %s\n", tmpChat.str);
				printf("[%s�� %d�� �濡���� ��ȭ] ", nickname, room_num);
				break;
			case pkt_type::pt_join:
				ZeroMemory(&tmpJoin, sizeof(t_join));
				memcpy((&tmpJoin) + sizeof(short), datum, remain);

				printf("%s���� %d�� �濡 �����ϼ̽��ϴ�. ", tmpJoin.nickname, tmpJoin.room_num);
				break;
			case pkt_type::pt_leave:
				ZeroMemory(&tmpLeave, sizeof(t_leave));
				memcpy((&tmpLeave) + sizeof(short), datum, remain);
				
				printf("%s���� %d�� �濡�� ����ϼ̽��ϴ�. ", tmpLeave.nickname, tmpLeave.room_num);
				break;
			}

			delete datum;

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
