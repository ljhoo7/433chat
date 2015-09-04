#include "headers.h"
#include "..\utilities.h"

int fps = 30;
double block = 1000 / fps;

extern int				room_num;

bool MyReceiveMessage(t_packet& tmppacket, const int retval);

DWORD WINAPI ReceivingThread(LPVOID arg)
{
	SOCKET sock = *(SOCKET*)arg;

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
			retval = 0;

			short length;
			retval = recvn(sock, (char*)&length, sizeof(short), 0);
			char *tmppakt = new char[length - 2];

			retval = recvn(sock, (char*)&tmppakt, length - 2, 0);

			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error");

			//MyReceiveMessage(tmppakt, retval);

			start_time = std::chrono::system_clock::now();
		}
	}
	return true;
}

bool MyReceiveMessage(t_packet& tmppacket, const int retval)
{
	switch (tmppacket.m_any.type)
	{
	case pkt_type::pt_chat:
		// 받은 데이터 출력
		//tmppacket.m_chat.str[retval] = '\0';
		//printf("\n[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		//printf("[받은 데이터] %s\n", tmppacket.m_chat.str);
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
