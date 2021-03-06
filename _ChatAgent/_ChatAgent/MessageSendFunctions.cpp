#include "stdafx.h"

extern CLogWriter *g_pLog;

bool CClient::SendCreateMessage(int num)
{
	if (num < 0)
	{
		std::cout << "The room number must be bigger than zero" << std::endl;
		return false;
	}

	// Creating Room Create Packet
	t_create tmp_packet;
	tmp_packet.type = pkt_type::pt_create;
	tmp_packet.room_num = num;

	m_pSock->Send((char*)&tmp_packet, sizeof(t_create));

	GetStateMachine()->ChangeState(CCreate_Response_Wait::Instance());

	return true;
}

bool CClient::SendDestroyMessage(int num)
{
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
	m_pSock->Send((char*)&tmp_packet, sizeof(t_destroy));

	GetStateMachine()->ChangeState(CDestroy_Response_Wait::Instance());

	return true;
}

bool CClient::SendJoinMessage(int num, char *nick)
{
	if (num < 0)
	{
		std::cout << "The room number must be bigger than zero" << std::endl;
		return false;
	}

	// Join Packet
	t_join tmp_packet;
	tmp_packet.type = pkt_type::pt_join;

	m_nTmpRoom_num = num;
	if (!SetTmpNickName(nick))
		std::cout << "SetNickName() length error" << std::endl;

	tmp_packet.room_num = num;
	strcpy((char*)tmp_packet.nickname, nick);

	// Sending Join Packet
	m_pSock->Send((char*)&tmp_packet, sizeof(t_join));

	GetStateMachine()->ChangeState(CJoin_Response_Wait::Instance());

	return true;
}

bool CClient::SendLeaveMessage()
{
	//	leave packet
	t_leave tmp_packet;
	int size = 20 + sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned int);
	tmp_packet.type = pkt_type::pt_leave;
	tmp_packet.room_num = m_nRoom_num;
	tmp_packet.token = m_nToken;
	strcpy((char*)tmp_packet.nickname, m_szNickname);

	// Sending leave packet
	m_pSock->Send((char*)&tmp_packet, sizeof(t_leave));

	m_nRoom_num = -1;

	GetStateMachine()->ChangeState(CLeave_Response_Wait::Instance());

	return true;
}

bool CClient::SendChatMessage(const std::string& str)
{
	t_chat tmp_packet;

	int size = sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)
		+ NICK_SIZE + sizeof(unsigned int);
	int strSize = str.size() + 1;

	char *buf = new char[size + strSize];

	tmp_packet.type = pkt_type::pt_chat;
	tmp_packet.length = size + strSize;

	tmp_packet.room_num = m_nRoom_num;
	tmp_packet.token = m_nToken;
	strcpy(tmp_packet.nickname, m_szNickname);

	memcpy(buf, (char*)&tmp_packet, size);

	// I had to use memcpy() which is the worst thing.
	strcpy(buf + size, str.c_str());

	// Sending Chat Messsage
	m_pSock->Send((char*)buf, size + strSize);

	return true;
}

bool CClient::SendEscapeMessage(DWORD destIp, int port)
{
	int retval;

	// �� : Must change to async !!!

	SOCKET	&sock = m_pSock->m_hSock;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		g_pLog->myWprintf(L"socket() in escaping !\n");
	}

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = destIp;
	serveraddr.sin_port = htons(port);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		g_pLog->myWprintf(L"connect() in escaping !\n");
	}

	return true;
}