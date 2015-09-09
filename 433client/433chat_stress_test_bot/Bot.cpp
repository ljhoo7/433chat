#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

char randStr[RANDSTR_SIZE][RANDSTR_LENG] =
{ "Hello World ~ ", "Hi Hi !!", "Nice to meet you", "I'm a programmer.", "So what ?"
, "Good luck", "Good morning ~" };

extern unsigned long		ip;
extern int					port;

DWORD WINAPI ReceivingThread(LPVOID arg);

CBot::CBot(const int& room_num, const int& time_span, const int& bot_num) : m_nRoom_num(room_num), m_pStateMachine(nullptr), hReceiving(NULL), sock(NULL), m_nToken(-1)
, m_nTmpRoom_num(-1)
, m_pReceivingThread(nullptr)
, m_nBot_num(bot_num)
{
	int retval;

	buf_str = "";
	strcpy_s(nickname, "");

	// initializing the windows socket
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NULL)
	{
		std::cout << "There is an error on the WSAStartup() of the " << bot_num << "th bot" << std::endl;

		exit(1);
	}

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit(const_cast<char*>(("socket() - " + std::to_string(bot_num)).c_str()));

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ip;//SERVERIP);
	serveraddr.sin_port = htons(port);//SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit(const_cast<char*>(("connect()" + std::to_string(bot_num)).c_str()));

	// starting the receivingi thread
	m_pReceivingThread = new std::thread(&CBot::ReceivingThread, this);

	// Initialize The State Machine
	m_pStateMachine = new StateMachine<CBot>(this);
	GetStateMachine()->SetCurrentState(CLobby::Instance());
}

CBot::~CBot()
{
	if (m_pReceivingThread != nullptr)
		delete m_pReceivingThread;

	if (sock != NULL)
		closesocket(sock);
	else
	{
		std::cout << "The thread handle is null !" << std::endl;
		std::cout << "The thread handle is null ! - " << m_nBot_num << "th bot" << std::endl;
	}

	// Windows sock close
	if (WSACleanup() != 0)
	{
		std::cout << "There is an error on the WSACleanup() of the " << m_nBot_num << "th bot" << std::endl;
		exit(1);
	}
}

void CBot::errorQuitWithBotNum(std::string str)
{
	err_quit(const_cast<char*>((str + std::to_string(m_nBot_num)).c_str()));
}

void CBot::ReceivingThread()
{
	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	int fps = 30;
	double block = 1000 / fps;

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
		if (block <= time)
		{

			if (GetStateMachine()->CurrentState() == CRoom::Instance())
				SendChatMessage(randStr[time % RANDSTR_SIZE]);

			t_create_failure		tmpCreateFailure;
			t_join_success			tmpJoinSuccess;
			t_join_failure			tmpJoinFailure;
			t_chat					tmpChatAlarm;
			t_join_alarm			tmpJoinAlarm;
			t_leave_alarm			tmpLeaveAlarm;
			t_kick					tmpKick;

			sum = 0;
			retval = 0;

			unsigned short type;

			retval = recvn(sock, (char*)&type, sizeof(unsigned short), 0);
			if (retval == SOCKET_ERROR)
				errorQuitWithBotNum("ReceivingThread() error on receiving the type.");
			sum += retval;

			if (GetStateMachine()->CurrentState() == CCreate_Response_Wait::Instance())
			{
				switch ((pkt_type)type)
				{
				case pkt_type::pt_create_success:

					m_nRoom_num = m_nTmpRoom_num;

					SendJoinMessage(m_nTmpRoom_num, const_cast<char*>(("bot_" + std::to_string(m_nBot_num)).c_str()));

					GetStateMachine()->ChangeState(CJoin_Response_Wait::Instance());

					break;
				case pkt_type::pt_create_failure:

					retval = recvn(sock, (char*)&tmpCreateFailure.fail_signal, sizeof(t_create_failure)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						errorQuitWithBotNum("ReceivingThread() error on the receiving the left of the t_create_failure.");
					sum += retval;

					printFailSignal((fail_signal)tmpCreateFailure.fail_signal);

					GetStateMachine()->ChangeState(CLobby::Instance());

					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Create Response' State." << std::endl;
					break;
				}
			}
			else if (GetStateMachine()->CurrentState() == CJoin_Response_Wait::Instance())
			{
				switch ((pkt_type)type)
				{
				case pkt_type::pt_join_success:
					retval = recvn(sock, (char*)&tmpJoinSuccess.trash_value, sizeof(t_join_success)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						errorQuitWithBotNum("ReceivingThread() error on the receiving the left of the t_join_success.");
					sum += retval;

					m_nToken = tmpJoinSuccess.token;

					m_nRoom_num = m_nTmpRoom_num;

					GetStateMachine()->ChangeState(CRoom::Instance());

					std::cout << std::to_string(m_nBot_num) << "Join Success" << std::endl;
					break;
				case pkt_type::pt_join_failure:

					retval = recvn(sock, (char*)&tmpJoinFailure.fail_signal, sizeof(t_join_failure)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						errorQuitWithBotNum("ReceivingThread() error on the receiving the left of the t_join_failure.");
					sum += retval;

					printFailSignal((fail_signal)tmpJoinFailure.fail_signal);

					SetNickName("");

					GetStateMachine()->ChangeState(CLobby::Instance());

					std::cout << std::to_string(m_nBot_num) << "Join Failed" << std::endl;
					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Join Response' State." << std::endl;
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
						errorQuitWithBotNum("ReceivingThread() error on the receiving the length of the t_chat_alarm.");
					sum += retval;

					remain = len - sizeof(unsigned short) - sizeof(unsigned short);// -sizeof(unsigned short);
					fixedRemain = 20 + sizeof(int) + sizeof(unsigned short);// +sizeof(unsigned short);
					strSize = remain - fixedRemain;

					char* buf = new char[remain];

					retval = recvn(sock, buf, remain, 0);
					if (retval == SOCKET_ERROR)
						errorQuitWithBotNum("ReceivingThread() error on the receiving the length of the t_chat_alarm.");
					sum += retval;

					memcpy(&tmpChatAlarm.room_num, buf, fixedRemain);

					char *tmpStr = new char[strSize];
					memcpy(tmpStr, &buf[fixedRemain], strSize);

					tmpChatAlarm.message = tmpStr;

					std::cout << "<" << tmpChatAlarm.nickname << ">'s message : " << tmpChatAlarm.message << ", "  <<  std::endl;

					break;
				}
				case pkt_type::pt_leave_alarm:

					retval = recvn(sock, (char*)&tmpLeaveAlarm.room_num, sizeof(t_leave_alarm)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						errorQuitWithBotNum("ReceivingThread() error on the receiving the left of the t_leave_alarm.");
					sum += retval;

					break;
				case pkt_type::pt_join_alarm:

					retval = recvn(sock, (char*)&tmpJoinAlarm.room_num, sizeof(t_join_alarm)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						errorQuitWithBotNum("ReceivingThread() error on the receiving the left of the t_join_alarm.");
					sum += retval;
					break;
				case pkt_type::pt_kick:

					retval = recvn(sock, (char*)&tmpKick, sizeof(t_kick)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						errorQuitWithBotNum("ReceivingThread() error on the receiving the left of the t_kick.");
					sum += retval;

					SetRoomNumber(-1);

					std::cout << "A Robot will not be kicked." + std::to_string(m_nBot_num) << std::endl;
					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Room' State-" + std::to_string(m_nBot_num) << std::endl;
					break;
				}
			}

			start_time = std::chrono::system_clock::now();
		}
	}
}

bool CBot::SendCreateMessage(int num)
{
	int retval;

	if (num < 0)
	{
		std::cout << "The room number must be bigger than zero" + std::to_string(m_nBot_num) << std::endl;
		return false;
	}

	// Creating Room Create Packet
	t_create tmp_packet;
	tmp_packet.type = pkt_type::pt_create;
	tmp_packet.room_num = num;

	// Sending the Generated Room Create Packet
	retval = send(GetSocket(), (char*)&tmp_packet, sizeof(t_create), 0);
	if (retval == SOCKET_ERROR){
		errorQuitWithBotNum("send() error on t_create");
	}

	return true;
}
bool CBot::SendChatMessage(const std::string& str)
{
	int retval;
	t_chat tmp_packet;

	int size = sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)//+sizeof(unsigned short)
		+20+sizeof(unsigned int);
	int strSize = str.size() + 1;

	char *buf = new char[size + strSize];

	tmp_packet.type = pkt_type::pt_chat;
	tmp_packet.length = size + strSize;

	tmp_packet.room_num = m_nRoom_num;
	tmp_packet.token = m_nToken;
	strcpy_s(tmp_packet.nickname, nickname);

	memcpy(buf, (char*)&tmp_packet, size);

	// I had to use memcpy() which is the worst thing.
	memcpy(buf + size, str.c_str(), strSize);

	// Sending Chat Messsage
	retval = send(sock, buf, tmp_packet.length, 0);
	if (retval == SOCKET_ERROR){
		errorQuitWithBotNum("send() error on t_chat !");
		return false;
	}

	return true;
}

bool CBot::SendJoinMessage(int num, char *nick)
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

	if (!SetNickName(nick))
		std::cout << "SetNickName() length error" << std::endl;

	tmp_packet.room_num = num;
	strcpy_s(tmp_packet.nickname, nick);

	// Sending Join Packet
	retval = send(GetSocket(), (char*)&tmp_packet, size, 0);
	if (retval == SOCKET_ERROR){
		errorQuitWithBotNum("send() error on t_join");
		return false;
	}

	GetStateMachine()->ChangeState(CJoin_Response_Wait::Instance());

	return true;
}