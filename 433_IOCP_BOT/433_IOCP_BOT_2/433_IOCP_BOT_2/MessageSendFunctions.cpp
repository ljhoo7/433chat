#include "stdafx.h"

extern CLogWriter	*g_pLog;

bool CBot::SendCreateMessage(int p_nNum)
{
	if (p_nNum < 0)
	{
		g_pLog->myWprintf("The room number must be bigger than zero\n");
		return false;
	}

	// Creating Room Create Packet
	t_create tmp_packet;
	tmp_packet.type = pkt_type::pt_create;
	tmp_packet.room_num = p_nNum;


	// Very Important !!!!! : The sequence of the sentences which are below must (1)->(2) !!!!
	GetStateMachine()->ChangeState(CCreate_Response_Wait::Instance()); // ---- (1)

	Send((char*)&tmp_packet, sizeof(t_create));								   // ---- (2)

	return true;
}

bool CBot::SendDestroyMessage(int p_nNum)
{
	if (m_nRoomNum == p_nNum)
	{
		g_pLog->myWprintf("You can't destroy the room where you have been chatting in\n");
		return false;
	}

	if (p_nNum < 0)
	{
		g_pLog->myWprintf("The room number must be bigger than zero\n");
		return false;
	}

	// Destroy Packet
	t_destroy tmp_packet;
	tmp_packet.type = pkt_type::pt_destroy;
	tmp_packet.room_num = p_nNum;

	// Very Important !!!!! : The sequence of the sentences which are below must (1)->(2) !!!!
	GetStateMachine()->ChangeState(CDestroy_Response_Wait::Instance()); // ---- (1)

	Send((char*)&tmp_packet, sizeof(t_destroy));								// ---- (2)

	return true;
}

bool CBot::SendJoinMessage(int p_nNum, char *p_szNick)
{
	if (p_nNum < 0)
	{
		std::cout << "The room number must be bigger than zero" << std::endl;
		return false;
	}

	// Join Packet
	t_join tmp_packet;
	tmp_packet.type = pkt_type::pt_join;

	m_nTmpRoomNum = p_nNum;
	
	if (!SetTmpNickName(p_szNick))
		std::cout << "SetNickName() length error" << std::endl;

	tmp_packet.room_num = p_nNum;
	strcpy((char*)tmp_packet.nickname, p_szNick);

	// Very Important !!!!! : The sequence of the sentences which are below must (1)->(2) !!!!
	GetStateMachine()->ChangeState(CJoin_Response_Wait::Instance()); // ---- (1)

	Send((char*)&tmp_packet, sizeof(t_join)); 			     // ---- (2)

	return true;
}

bool CBot::SendLeaveMessage()
{
	//	leave packet
	t_leave tmp_packet;
	int size = 20 + sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned int);
	tmp_packet.type = pkt_type::pt_leave;
	tmp_packet.room_num = m_nRoomNum;
	tmp_packet.token = m_nToken;
	strcpy((char*)tmp_packet.nickname, m_szNick);

	// Very Important !!!!! : The sequence of the sentences which are below must (1)->(2) !!!!
	GetStateMachine()->ChangeState(CLeave_Response_Wait::Instance()); // ---- (1)

	Send((char*)&tmp_packet, sizeof(t_leave)); 			  // ---- (2)

	m_nRoomNum = -1;

	return true;
}

bool CBot::SendChatMessage(const std::string& p_sStr)
{
	t_chat tmp_packet;

	int size = sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)
		+ NICK_SIZE + sizeof(unsigned int);
	int strSize = p_sStr.size() + 1;

	char *buf = new char[size + strSize];

	tmp_packet.type = pkt_type::pt_chat;
	tmp_packet.length = size + strSize;

	tmp_packet.room_num = m_nRoomNum;
	tmp_packet.token = m_nToken;
	strcpy(tmp_packet.nickname, m_szNick);

	memcpy(buf, (char*)&tmp_packet, size);

	// I had to use memcpy() which is the worst thing.
	strcpy(buf + size, p_sStr.c_str());

	// Sending Chat Messsage
	Send((char*)buf, size + strSize);

	return true;
}

bool CBot::SendEscapeSuccessMessage()
{
	t_escape_success tmp_packet;

	tmp_packet.type = pkt_type::pt_escape_success;

	Send((char*)&tmp_packet, sizeof(t_escape_success));

	return true;
}

bool CBot::SendEscapeFailureMessage()
{
	t_escape_failure tmp_packet;

	tmp_packet.type = pkt_type::pt_escape_failure;

	Send((char*)&tmp_packet, sizeof(t_escape_failure));

	return true;
}