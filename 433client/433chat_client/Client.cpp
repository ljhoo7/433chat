#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

DWORD WINAPI ReceivingThread(LPVOID arg);

CClient::CClient() : m_nRoom_num(-1), m_pStateMachine(nullptr), hReceiving(NULL), sock(NULL), m_nToken(-1)
, m_nTmpJoinRoom_num(-1)
, m_pReceivingThread(nullptr)
{
	strcpy(nickname, "");

	// local variables
	int retval, port;
	unsigned long ip;
	std::string buf_str, wanted_str;

	while (1)
	{
		std::cout << "[input the ip address to be connected] ";
		std::getline(std::cin, buf_str);

		if (buf_str.size() > 0)
			ip = inet_addr(buf_str.substr(0, buf_str.size()).c_str());
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
		std::cout << "[input the port number of the server] ";
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
	// initializing the windows socket
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

	// starting the receivingi thread
	m_pReceivingThread = new std::thread(&CClient::ReceivingThread, this);

	// Initialize The State Machine
	m_pStateMachine = new StateMachine<CClient>(this);
	GetStateMachine()->SetCurrentState(CLobby::Instance());
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

	char *buf;

	long long time;
	int retval = 0;
	std::string tmpStr;
	int size = 0, sum = 0, remain = 0, fixedRemain, strSize;
	unsigned short len;

	while (1)
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){
			t_create_success		tmpCreateSuccess;
			t_create_failure		tmpCreateFailure;
			t_destroy_success		tmpDestroySuccess;
			t_destroy_failure		tmpDestroyFailure;
			t_join_success			tmpJoinSuccess;
			t_join_failure			tmpJoinFailure;
			t_leave_success			tmpLeaveSuccess;
			t_chat					tmpChatAlarm;
			t_join_alarm			tmpJoinAlarm;
			t_leave_alarm			tmpLeaveAlarm;
			t_kick					tmpKick;

			char *left;

			sum = 0;
			retval = 0;

			unsigned short type;

			retval = recvn(sock, (char*)&type, sizeof(unsigned short), 0);
			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error on receiving the type.");
			sum += retval;

			if (GetStateMachine()->CurrentState() == CCreate_Response_Wait::Instance())
			{
				switch ((pkt_type)type)
				{
				case pkt_type::pt_create_success:

					std::cout << "Successfully created !!" << std::endl;

					GetStateMachine()->ChangeState(CLobby::Instance());

					break;
				case pkt_type::pt_create_failure:

					retval = recvn(sock, (char*)&tmpCreateFailure.fail_signal, sizeof(t_create_failure)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_create_failure.");
					sum += retval;

					printFailSignal((fail_signal)tmpCreateFailure.fail_signal);

					GetStateMachine()->ChangeState(CLobby::Instance());

					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Create Response' State." << std::endl;
					break;
				}
			}
			else if (GetStateMachine()->CurrentState() == CDestroy_Response_Wait::Instance())
			{
				switch ((pkt_type)type)
				{
				case pkt_type::pt_destroy_success:

					std::cout << "Successfully destroyed !!" << std::endl;

					break;
				case pkt_type::pt_destroy_failure:

					retval = recvn(sock, (char*)&tmpDestroyFailure.fail_signal, sizeof(t_destroy_failure)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_destroy_failure.");
					sum += retval;

					printFailSignal((fail_signal)tmpDestroyFailure.fail_signal);

					GetStateMachine()->ChangeState(CLobby::Instance());

					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Destroy Response' State." << std::endl;
					break;
				}
			}
			else if (GetStateMachine()->CurrentState() == CJoin_Response_Wait::Instance())
			{
				switch ((pkt_type)type)
				{
				case pkt_type::pt_join_success:
					retval = recvn(sock, (char*)&tmpJoinSuccess.trash_value, sizeof(t_join_success) - sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_join_success.");
					sum += retval;

					m_nToken = tmpJoinSuccess.token;

					m_nRoom_num = m_nTmpJoinRoom_num;

					std::cout << "Successfully joined !!" << std::endl;

					GetStateMachine()->ChangeState(CRoom::Instance());
					break;
				case pkt_type::pt_join_failure:

					retval = recvn(sock, (char*)&tmpJoinFailure.fail_signal, sizeof(t_join_failure)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_join_failure.");
					sum += retval;

					printFailSignal((fail_signal)tmpJoinFailure.fail_signal);

					SetNickName("");

					GetStateMachine()->ChangeState(CLobby::Instance());
					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Join Response' State." << std::endl;
					break;
				}
			}
			else if (GetStateMachine()->CurrentState() == CLeave_Response_Wait::Instance())
			{
				switch ((pkt_type)type)
				{
				case pkt_type::pt_leave_success:
					retval = recvn(sock, (char*)&tmpLeaveSuccess, sizeof(t_leave_success)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_leave_success.");
					sum += retval;

					SetRoomNumber(-1);

					std::cout << "Successfully leaved !!" << std::endl;

					GetStateMachine()->ChangeState(CLobby::Instance());
					break;
				case pkt_type::pt_leave_alarm:

					retval = recvn(sock, (char*)&tmpLeaveAlarm.room_num, sizeof(t_leave_alarm)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_leave_alarm.");
					sum += retval;

					std::cout << "'" << tmpLeaveAlarm.nickname << "'has been leaved from your room." << std::endl;
					
					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Leave Response' State." << std::endl;
					break;
				}
			}
			else if (GetStateMachine()->CurrentState() == CRoom::Instance())
			{
				switch ((pkt_type)type)
				{
				case pkt_type::pt_chat_alarm:
				{
					retval = recvn(sock, (char*)&len, sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the length of the t_chat_alarm.");
					sum += retval;

					remain = len - sizeof(unsigned short) - sizeof(unsigned short);// -sizeof(unsigned short);
					fixedRemain = 20 + sizeof(int) + sizeof(unsigned short);// +sizeof(unsigned short);
					strSize = remain - fixedRemain;

					char* buf = new char[remain];

					retval = recvn(sock, buf, remain, 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the length of the t_chat_alarm.");
					sum += retval;

					memcpy(&tmpChatAlarm.room_num, buf, fixedRemain);

					char *tmpStr = new char[strSize];
					memcpy(tmpStr, &buf[fixedRemain], strSize);

					tmpChatAlarm.message = tmpStr;

					if (tmpChatAlarm.room_num == m_nRoom_num)
					{
						std::cout << "<" << tmpChatAlarm.nickname << ">'s message : " << tmpChatAlarm.message << std::endl;
					}

					break;
				}
				case pkt_type::pt_join_alarm:

					retval = recvn(sock, (char*)&tmpJoinAlarm.room_num, sizeof(t_join_alarm)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_join_alarm.");
					sum += retval;

					if (tmpJoinAlarm.room_num == m_nRoom_num)
					{
						std::cout << "'" << tmpJoinAlarm.nickname << "'has entered to your room." << std::endl;
					}

					break;
				case pkt_type::pt_kick:

					retval = recvn(sock, (char*)&tmpKick, sizeof(t_kick)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_kick.");
					sum += retval;

					SetRoomNumber(-1);

					std::cout << "You are expeled because your room was erased." << std::endl;

					GetStateMachine()->ChangeState(CLobby::Instance());
					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Room' State." << std::endl;
					break;
				}
			}

			std::cout << sum << "bytes were received." << std::endl;

			start_time = std::chrono::system_clock::now();
		}
	}
}

bool CClient::SendCreateMessage(int num)
{
	int retval;

	if (num < 0)
	{
		std::cout << "The room number must be bigger than zero" << std::endl;
		return false;
	}

	// Creating Room Create Packet
	t_create tmp_packet;
	tmp_packet.type = pkt_type::pt_create;
	tmp_packet.room_num = num;

	// Sending the Generated Room Create Packet
	retval = send(GetSocket(), (char*)&tmp_packet, sizeof(t_create), 0);
	if (retval == SOCKET_ERROR){
		err_display("send() error on t_create");
		return false;
	}

	printf("[TCP client] %dbytes were sent.\n", retval);

	GetStateMachine()->ChangeState(CCreate_Response_Wait::Instance());

	return true;
}

bool CClient::SendDestroyMessage(int num)
{
	int retval;

	if (GetRoomNumber() == num)
	{
		std::cout << "You can't destroy the room where you have been chatting in" << std::endl;
		return false;
	}

	if (num < 0)
	{
		std::cout << "The room number must be bigger than zero" << std::endl;
		return false;
	}

	// Destroy Packet
	t_destroy tmp_packet;
	tmp_packet.type = pkt_type::pt_destroy;
	tmp_packet.room_num = num;

	// Sending Destroy Packet
	retval = send(sock, (char*)&tmp_packet, sizeof(t_destroy), 0);
	if (retval == SOCKET_ERROR){
		err_display("send() error on t_destroy");
		return false;
	}

	printf("[TCP client] %dbytes were sent.\n", retval);

	GetStateMachine()->ChangeState(CDestroy_Response_Wait::Instance());

	return true;
}
bool CClient::SendJoinMessage(int num, char *nick)
{
	int retval;

	if (num < 0)
	{
		std::cout << "The room number must be bigger than zero" << std::endl;
		return false;
	}

	// Join Packet
	t_join tmp_packet;
	int size = 20 + sizeof(unsigned short)+sizeof(unsigned short);
	tmp_packet.type = pkt_type::pt_join;

	m_nTmpJoinRoom_num = num;
	if(!SetNickName(nick))
		std::cout << "SetNickName() length error" << std::endl;

	tmp_packet.room_num =  num;
	strcpy(tmp_packet.nickname, nick);

	// Sending Join Packet
	retval = send(GetSocket(), (char*)&tmp_packet, size, 0);
	if (retval == SOCKET_ERROR){
		err_display("send() error on t_join");
		return false;
	}

	printf("[TCP client] %dbytes were sent.\n", retval);

	GetStateMachine()->ChangeState(CJoin_Response_Wait::Instance());

	return true;
}
bool CClient::SendLeaveMessage()
{
	int retval;

	//	leave packet
	t_leave tmp_packet;
	int size = 20 + sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned int);
	tmp_packet.type = pkt_type::pt_leave;
	tmp_packet.room_num = m_nRoom_num;
	tmp_packet.token = m_nToken;
	strcpy(tmp_packet.nickname, nickname);

	// Sending leave packet
	retval = send(sock, (char*)&tmp_packet, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send() error on t_leave");
		return false;
	}

	printf("[TCP client] %dbytes were sent.\n", retval);

	SetRoomNumber(-1);

	GetStateMachine()->ChangeState(CLeave_Response_Wait::Instance());

	return true;
}
bool CClient::SendChatMessage(const std::string& str)
{
	int retval;
	t_chat tmp_packet;

	int size = sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)//+sizeof(unsigned short)
		+20+sizeof(unsigned int);
	int strSize = str.size() + 1;

	char *buf = new char[size + strSize];

	tmp_packet.type = pkt_type::pt_chat;
	tmp_packet.length = size + strSize;

	printf("the length of chat message : %d\n", tmp_packet.length);
	tmp_packet.room_num = m_nRoom_num;
	tmp_packet.token = m_nToken;
	strcpy(tmp_packet.nickname, nickname);

	memcpy(buf, (char*)&tmp_packet, size);

	// I had to use memcpy() which is the worst thing.
	memcpy(buf + size, str.c_str(), strSize);

	// Sending Chat Messsage
	retval = send(sock, buf, tmp_packet.length, 0);
	if (retval == SOCKET_ERROR){
		err_display("send() error on t_chat !");
		return false;
	}

	return true;
}