#include "headers.h"
#include "..\utilities.h"

int fps = 30;
double block = 1000 / fps;

extern SOCKET			sock;
extern CReceiver		*g_cReceivingManager;

extern char				*bot_name;
extern int				room_num;

DWORD WINAPI ReceivingThread(LPVOID arg)
{
	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	long long time;
	int retval = 0;
	int size = 0;

	while (1)
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){
			t_packet tmppakt;
			retval = 0;

			retval = recvn(sock, (char*)&tmppakt, sizeof(t_packet), 0);

			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error");

			g_cReceivingManager->MyReceiveMessage(tmppakt, retval);

			start_time = std::chrono::system_clock::now();
		}
	}
	return true;
}

bool CReceiver::MyReceiveMessage(t_packet& tmppacket, const int retval)
{
	switch (tmppacket.m_any.type)
	{
	case pkt_type::pt_chat:
		// ���� ������ ���
		tmppacket.m_chat.str[retval] = '\0';
		printf("\n[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", tmppacket.m_chat.str);
		printf("['%s'�� ���� ������] ", bot_name);
		break;
	}
	return true;
}


CReceiver::CReceiver(void)
{
}


CReceiver::~CReceiver(void)
{
}
