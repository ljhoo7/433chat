#include "Headers.h"
#include "..\Utilities.h"

DWORD WINAPI ReceivingThread(LPVOID arg);

CClient::CClient() : room_num(-1), m_pStateMachine(nullptr), nickname(""), hReceiving(NULL), sock(NULL)
, m_pReceivingThread(nullptr)
{
	// Initialize The State Machine
	m_pStateMachine = new StateMachine<CClient>(this);
	GetStateMachine()->SetCurrentState(CNotConnected::Instance());

	// local variables
	int retval, port;
	unsigned long ip;
	std::string buf_str, wanted_str;

	while (1)
	{
		std::cout << "[접속할 서버의 ip주소] ";
		std::getline(std::cin, buf_str);

		if (buf_str.size() > 0)
			ip = inet_addr(buf_str.substr(0, buf_str.size()-1).c_str());
		else
		{
			std::cout << "Error : The size of the input buffer is a zero or a negative number." << std::endl;
		}
		
		if (ip != INADDR_NONE)
		{
			break;
		}
		else
		{
			std::cout << "Error : This is not a legitimate Internet address." << std::endl;
		}
	}

	while (1)
	{
		std::cout << "[접속할 서버의 port] ";
		std::getline(std::cin, buf_str);

		if (buf_str.size() > 0)
			port = stoi(buf_str);
		else
		{
			std::cout << "Error : The size of the input buffer is a zero or a negative number." << std::endl;
		}
		

		if (port < 1025 || port > 65535)
		{
			std::cout << "The rang of port number must be in 1025 ~ 65535" << std::endl;
		}
		else
		{
			break;
		}
	}

	// 리시버 스레드 구동
	m_pReceivingThread = new std::thread(ReceivingThread);

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NULL)
	{
		std::cout << "There is an error on WSAStartup() << std::endl";

		exit(1);
	}

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ip;//SERVERIP);
	serveraddr.sin_port = htons(port);//SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
}

CClient::~CClient()
{
	if (m_pReceivingThread != nullptr)
		delete m_pReceivingThread;

	if (sock != NULL)
		closesocket(sock);
	else
	{
		std::cout << "The thread handle is null !" << std::endl;
	}

	// Windows sock close
	if (WSACleanup() != 0)
	{
		std::cout << "An error occured on WASCleanup()." << std::endl;
		exit(1);
	}
}

void CClient::ReceivingThread()
{
	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	int fps = 30;
	double block = 1000 / fps;

	long long time;
	int retval = 0;
	int size = 0, sum = 0;

	t_join				tmpJoin;
	t_chat				tmpChat;
	t_leave				tmpLeave;
	t_create			tmpCreate;

	while (1)
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){

			sum = 0;
			retval = 0;

			short type;

			retval = recvn(sock, (char*)&type, sizeof(unsigned short), 0);
			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error on receiving the type.");
			sum += retval;

			switch ((pkt_type)type)
			{
			case pkt_type::pt_create:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_create) - sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_create.");
				sum += retval;

				break;
			case pkt_type::pt_create_success:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_create_success)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_create_success.");
				sum += retval;

				break;
			case pkt_type::pt_create_failure:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_create_failure)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_create_failure.");
				sum += retval;

				break;
			case pkt_type::pt_destroy:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_destroy)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_destroy.");
				sum += retval;

				break;
			case pkt_type::pt_destroy_success:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_destroy_success)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_destroy_success.");
				sum += retval;

				break;
			case pkt_type::pt_destroy_failure:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_destroy_failure)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_destroy_failure.");
				sum += retval;

				break;
			case pkt_type::pt_join:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_join)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_join.");
				sum += retval;

				break;
			case pkt_type::pt_join_success:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_join_success)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_join_success.");
				sum += retval;

				break;
			case pkt_type::pt_join_failure:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_join_failure)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_join_failure.");
				sum += retval;

				break;
			case pkt_type::pt_leave:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_leave)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_leave.");
				sum += retval;

				break;
			case pkt_type::pt_leave_success:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_leave_success)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_leave_success.");
				sum += retval;

				break;
			case pkt_type::pt_chat_alarm:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_chat_alarm)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_chat_alarm.");
				sum += retval;

				break;
			case pkt_type::pt_chat:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_chat)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_chat.");
				sum += retval;

				break;
			case pkt_type::pt_join_alarm:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_join_alarm)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_join_alarm.");
				sum += retval;

				break;
			case pkt_type::pt_leave_alarm:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_leave_alarm)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_leave_alarm.");
				sum += retval;

				break;
			case pkt_type::pt_kick:

				retval = recvn(sock, (char*)&tmpCreate, sizeof(t_kick)-sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_kick.");
				sum += retval;

				break;
			}

			start_time = std::chrono::system_clock::now();
		}
	}
}
