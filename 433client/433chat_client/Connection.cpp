#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

extern int t_cnt;

DWORD WINAPI ReceivingThread(LPVOID arg);

CConnection::CConnection(unsigned long ip, unsigned short port, char* nick, int rNum)
: m_pStateMachine(nullptr), m_pReceivingThread(nullptr)
,sock(NULL), m_nToken(-1)
{
	t_cnt++;
	int retval;

	if (NULL == nick)
		strcpy_s(nickname, "");

	if (-1 == rNum)
		m_nRoom_num = -1;
	else
	{

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

	// starting the receiving thread
	m_pReceivingThread = new std::thread(&CConnection::ReceivingThread, this);

	// Initialize The State Machine
	m_pStateMachine = new StateMachine<CConnection>(this);
	GetStateMachine()->SetCurrentState(CLobby::Instance());
}

CConnection::~CConnection()
{
	t_cnt--;
}


bool CConnection::SendCreateMessage(int num)
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
	retval = send(sock, (char*)&tmp_packet, sizeof(t_create), 0);
	if (retval == SOCKET_ERROR){
		err_display("send() error on t_create");
		return false;
	}

	GetStateMachine()->ChangeState(CCreate_Response_Wait::Instance());

	return true;
}

bool CConnection::SendDestroyMessage(int num)
{
	int retval;

	if (m_nRoom_num == num)
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

	GetStateMachine()->ChangeState(CDestroy_Response_Wait::Instance());

	return true;
}

bool CConnection::SendJoinMessage(int num, char *nick)
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
	retval = send(sock, (char*)&tmp_packet, size, 0);
	if (retval == SOCKET_ERROR){
		err_display("send() error on t_join");
		return false;
	}

	GetStateMachine()->ChangeState(CJoin_Response_Wait::Instance());

	return true;
}

bool CConnection::SendLeaveMessage()
{
	int retval;

	//	leave packet
	t_leave tmp_packet;
	int size = 20 + sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned int);
	tmp_packet.type = pkt_type::pt_leave;
	tmp_packet.room_num = m_nRoom_num;
	tmp_packet.token = m_nToken;
	strcpy_s(tmp_packet.nickname, nickname);

	// Sending leave packet
	retval = send(sock, (char*)&tmp_packet, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send() error on t_leave");
		return false;
	}

	m_nRoom_num = -1;

	GetStateMachine()->ChangeState(CLeave_Response_Wait::Instance());

	return true;
}

bool CConnection::SendChatMessage(const std::string& str)
{
	int retval;
	t_chat tmp_packet;

	int size = sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)//+sizeof(unsigned short)
		+20 + sizeof(unsigned int);
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
		err_display("send() error on t_chat !");
		return false;
	}

	return true;
}

bool CConnection::Escaping(unsigned long destIp, unsigned short port)
{
	int retval;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = destIp;
	serveraddr.sin_port = htons(port);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// starting the receiving thread
	m_pReceivingThread = new std::thread(&CConnection::ReceivingThread, this);

	// Initialize The State Machine
	m_pStateMachine = new StateMachine<CConnection>(this);
	GetStateMachine()->SetCurrentState(CLobby::Instance());

	return true;
}

void CConnection::ReceivingThread()
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
			t_create_failure		tmpCreateFailure;
			t_destroy_failure		tmpDestroyFailure;
			t_join_success			tmpJoinSuccess;
			t_join_failure			tmpJoinFailure;
			t_leave_success			tmpLeaveSuccess;
			t_chat					tmpChatAlarm;
			t_join_alarm			tmpJoinAlarm;
			t_leave_alarm			tmpLeaveAlarm;
			t_kick					tmpKick;
			t_user_out				tmpUserOut;
			t_escape_server			tmpEscapeServer;

			sum = 0;
			retval = 0;

			unsigned short type;

			retval = recvn(sock, (char*)&type, sizeof(unsigned short), 0);
			if (retval == SOCKET_ERROR)
				err_quit("ReceivingThread() error on receiving the type.");
			sum += retval;

			if ( pkt_type::pt_user_out == (pkt_type)type )
			{
				retval = recvn(sock, (char*)&tmpUserOut.trashValue, sizeof(t_user_out) - sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_user_out.");

				std::cout << "This has been received a user out message : socket - " << tmpUserOut.client_socket << std::endl;

				retval = closesocket(this->sock);
				if (retval)
				{
					switch (WSAGetLastError())
					{
					case WSANOTINITIALISED:
						std::cout << "WSANOTINITIALISED" << std::endl;
						break;
					case WSAENETDOWN:
						std::cout << "WSAENETDOWN" << std::endl;
						break;
					case WSAENOTSOCK:
						std::cout << "WSAENETSOCK" << std::endl;
						break;
					case WSAEINPROGRESS:
						std::cout << "WSAEINPROGRESS" << std::endl;
						break;
					case WSAEINTR:
						std::cout << "WSAEINTR" << std::endl;
						break;
					case WSAEWOULDBLOCK:
						std::cout << "WSAEWOULDBLOCK" << std::endl;
						break;
					}
				}
				//delete this;
				break;
			}
			else if ( pkt_type::pt_escape_server == (pkt_type)type )
			{
				retval = recvn(sock, (char*)&tmpEscapeServer.trashValue, sizeof(t_escape_server) - sizeof(unsigned short), 0);
				if (retval == SOCKET_ERROR)
					err_quit("ReceivingThread() error on the receiving the left of the t_escape_server.");

				std::cout << "This has been received a escape to server ( ip : " << tmpEscapeServer.dest_ip << ", port : "
					<< tmpEscapeServer.port << ") message." << std::endl;
			}
			else if (GetStateMachine()->CurrentState() == CCreate_Response_Wait::Instance())
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

					GetStateMachine()->ChangeState(CLobby::Instance());

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
					retval = recvn(sock, (char*)&tmpJoinSuccess.trash_value, sizeof(t_join_success)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_join_success.");
					sum += retval;

					m_nToken = tmpJoinSuccess.token;

					m_nRoom_num = m_nTmpRoom_num;

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

					m_nRoom_num = -1;

					std::cout << "Successfully leaved !!" << std::endl;

					GetStateMachine()->ChangeState(CLobby::Instance());
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

					remain = len - sizeof(unsigned short)-sizeof(unsigned short);// -sizeof(unsigned short);
					fixedRemain = 20 + sizeof(int)+sizeof(unsigned short);// +sizeof(unsigned short);
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
				case pkt_type::pt_leave_alarm:

					retval = recvn(sock, (char*)&tmpLeaveAlarm.room_num, sizeof(t_leave_alarm)-sizeof(unsigned short), 0);
					if (retval == SOCKET_ERROR)
						err_quit("ReceivingThread() error on the receiving the left of the t_leave_alarm.");
					sum += retval;

					std::cout << "'" << tmpLeaveAlarm.nickname << "'has been leaved from your room." << std::endl;

					break;
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

					m_nRoom_num = -1;

					std::cout << "You are kicked to the Lobby because your room was erased." << std::endl;

					GetStateMachine()->ChangeState(CLobby::Instance());
					break;
				default:
					std::cout << "You have received a wrong message which you can't read in the 'Room' State." << std::endl;
					break;
				}
			}

			start_time = std::chrono::system_clock::now();
		}
	}
	delete this;
}
