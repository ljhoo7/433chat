#include "stdafx.h"

extern CLogWriter		*g_pLog;
extern CProactor		*g_pProactor;

bool CreateSuccessFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	g_pLog->myWprintf("Room Creation Successeded !\n");

	return true;
}

bool CreateFailureFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_create_failure *t_sCreateFail = (t_create_failure*)t_pBot->m_szReceiveBuf;

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	g_pLog->myWprintf("[error] Room Creation Failed !\n");
	PrintFailSignal((fail_signal)t_sCreateFail->fail_signal);

	return true;
}

bool DestroySuccessFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	g_pLog->myWprintf("Room Destroy Successeded !\n");

	return true;
}

bool DestroyFailureFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_destroy_failure *t_sDestroyFail = (t_destroy_failure*)t_pBot->m_szReceiveBuf;

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	g_pLog->myWprintf("[error] Room Destroy Failed !\n");
	PrintFailSignal((fail_signal)t_sDestroyFail->fail_signal);

	return true;
}

bool JoinSuccessFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_join_success *t_sJoinSuccess = (t_join_success*)t_pBot->m_szReceiveBuf;

	t_pBot->m_nToken = t_sJoinSuccess->token;

	t_pBot->SetNickName(t_pBot->m_szTmpNick);
	t_pBot->m_nRoomNum = t_pBot->m_nTmpRoomNum;

	t_pBot->GetStateMachine()->ChangeState(CRoom::Instance());

	g_pLog->myWprintf("Join Successeded !\n");

	return true;
}

bool JoinFailureFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_join_failure *t_sJoinFail = (t_join_failure*)t_pBot->m_szReceiveBuf;

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	g_pLog->myWprintf("[error] Join Failed !\n");
	PrintFailSignal((fail_signal)t_sJoinFail->fail_signal);

	return true;
}

bool LeaveSuccessFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_pBot->m_nRoomNum = -1;
	t_pBot->m_nTmpRoomNum = -1;

	strcpy(t_pBot->m_szNick, "");
	strcpy(t_pBot->m_szTmpNick, "");

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	g_pLog->myWprintf("Leave Successeded !\n");

	return true;
}

bool ChatAlarmFunc(PVOID p_pParam)
{
	const int t_nTrashSize = 2;
	const int t_nHeaderAndLeng = HEADER_SIZE << 1;

	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	int t_nRemain = t_pBot->m_nRecvRemain - sizeof(unsigned short)-sizeof(unsigned short);;
	int t_nFixedRemain = NICK_SIZE + sizeof(int)+sizeof(unsigned short);
	int t_nStrSize = t_nRemain - t_nFixedRemain - t_nTrashSize;

	t_chat_alarm t_pChatAlarm;

	memcpy(&t_pChatAlarm.room_num, &t_pBot->m_szReceiveBuf[t_nHeaderAndLeng], t_nFixedRemain);

	char *t_szStr = new char[t_nStrSize];
	memcpy(t_szStr, &t_pBot->m_szReceiveBuf[t_nFixedRemain + t_nTrashSize + t_nHeaderAndLeng], t_nStrSize);

	std::string message = t_szStr;

	if (t_pChatAlarm.room_num == t_pBot->m_nRoomNum)
	{
		printf(" < %s >'s message : %s\n", t_pChatAlarm.nickname, message.c_str());
	}
	else
	{
		g_pLog->myWprintf("[Server's error] A chat alarm message of unassociated room has been received.\n");
	}

	return true;
}

bool JoinAlarmFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_join_alarm *t_sJoinAlarm = (t_join_alarm*)t_pBot->m_szReceiveBuf;

	if (t_sJoinAlarm->room_num == t_pBot->m_nRoomNum)
	{
		printf("< %s > has entered to your room.\n", t_sJoinAlarm->nickname);
	}
	else
	{
		g_pLog->myWprintf("[Server's error] A join alarm message of unassociated room has been received.\n");
	}

	return true;
}

bool LeaveAlarmFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_leave_alarm *t_sLeaveAlarm = (t_leave_alarm*)t_pBot->m_szReceiveBuf;

	if (t_sLeaveAlarm->room_num == t_pBot->m_nRoomNum)
	{
		printf("< %s > has leaved from your room.\n", t_sLeaveAlarm->nickname);
	}
	else
	{
		g_pLog->myWprintf("[Server's error] A leave alarm message of unassociated room has been received.\n");
	}

	return true;
}

bool KickFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_kick *t_sKick = (t_kick*)t_pBot->m_szReceiveBuf;

	t_pBot->m_nRoomNum = -1;
	t_pBot->m_nTmpRoomNum = -1;

	g_pLog->myWprintf("You are kicked to the Lobby because your room was erased !\n");

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	return true;
}

//--------------------------------------------------------

bool UserOutFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	g_pLog->myWprintf("An UserOut Message is received...\n");

	t_pBot->GetStateMachine()->ChangeState(CLobby::Instance());

	t_pBot->Disconnect();

	//exit(1);

	return true;
}

bool EscapeServerFunc(PVOID p_pParam)
{
	CAct *t_pAct = (CAct*)p_pParam;
	CBot *t_pBot = t_pAct->m_pBot;

	t_pBot->GetStateMachine()->ChangeState(CEscaping::Instance());

	t_escape_server *t_sEscapingServer = (t_escape_server*)t_pBot->m_szReceiveBuf;

	t_pBot->Connect(g_pProactor->m_cMswsock, t_sEscapingServer->dest_ip, t_sEscapingServer->port);

	return true;
}

CReceiver::CReceiver(CProactor *p_pProactor)
:m_pProactor(p_pProactor)
{
	m_pProactor = p_pProactor;

	// create
	PacketHandlingFunc[0] = NULL;

	// create_success
	PacketHandlingFunc[1] = CreateSuccessFunc;

	// create_failure
	PacketHandlingFunc[2] = CreateFailureFunc;

	// destroy
	PacketHandlingFunc[3] = NULL;

	// destroy success
	PacketHandlingFunc[4] = DestroySuccessFunc;

	// destroy failure
	PacketHandlingFunc[5] = DestroyFailureFunc;

	// join
	PacketHandlingFunc[6] = NULL;

	// join success
	PacketHandlingFunc[7] = JoinSuccessFunc;

	// join failure
	PacketHandlingFunc[8] = JoinFailureFunc;

	// leave
	PacketHandlingFunc[9] = NULL;

	// leave success
	PacketHandlingFunc[10] = LeaveSuccessFunc;

	// chat
	PacketHandlingFunc[11] = NULL;

	// chat alarm
	PacketHandlingFunc[12] = ChatAlarmFunc;

	// join alarm
	PacketHandlingFunc[13] = JoinAlarmFunc;

	// leave alarm
	PacketHandlingFunc[14] = LeaveAlarmFunc;

	// kick
	PacketHandlingFunc[15] = KickFunc;

	// user out
	PacketHandlingFunc[16] = UserOutFunc;

	// escape server
	PacketHandlingFunc[17] = EscapeServerFunc;

	// escape success
	PacketHandlingFunc[18] = NULL;

	// escape failure
	PacketHandlingFunc[19] = NULL;
}


CReceiver::~CReceiver()
{
}

void CReceiver::ProcError(CAct *p_pAct, DWORD p_dwError)
{
}

void CReceiver::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	CBot	&t_cBot = *p_pAct->m_pBot;
	SOCKET	&t_hSock = p_pAct->m_pBot->m_hSock;
	if (0 == p_dwTransferredBytes)
	{
		t_cBot.Disconnect();
		return;
	}

	int &t_nPos = t_cBot.m_nRecvPosition;
	t_nPos += p_dwTransferredBytes;
	int &t_nRemain = t_cBot.m_nRecvRemain;
	t_nRemain -= p_dwTransferredBytes;

	char *buf = t_cBot.m_szReceiveBuf;

	bool check = true;

	if (HEADER_SIZE <= t_nPos)
	{
		if (HEADER_SIZE == t_nPos)
		{
			pkt_type t_eType = (pkt_type)((unsigned short)(*buf));

			State<CBot> *t_pState = t_cBot.GetStateMachine()->CurrentState();

			if (pkt_type::pt_user_out == t_eType)
			{
				g_pLog->myWprintf("pt_user_out\n");
				t_nRemain = sizeof(t_user_out)-HEADER_SIZE;
				t_cBot.m_eType = pkt_type::pt_user_out;
			}
			else if (pkt_type::pt_escape_server == t_eType)
			{
				g_pLog->myWprintf("pt_escape_server\n");
				t_nRemain = sizeof(t_escape_server)-HEADER_SIZE;
				t_cBot.m_eType = pkt_type::pt_escape_server;
				//g_pClient->m_hOldSock = p_pAct->m_pSock->m_hSock;
			}
			else if (pkt_type::pt_cs_health_check == t_eType)
			{
				g_pLog->myWprintf("pt_health_check\n");
				t_nRemain = sizeof(t_health_check)-HEADER_SIZE;
				t_cBot.m_eType = pkt_type::pt_cs_health_check;
				//g_pClient->m_hOldSock = p_pAct->m_pSock->m_hSock;
			}
			else if (CCreate_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_create_success:
					g_pLog->myWprintf("pt_create_success\n");
					t_nRemain = sizeof(t_create_success)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_create_success;
					break;
				case pkt_type::pt_create_failure:
					g_pLog->myWprintf("pt_create_failure\n");
					t_nRemain = sizeof(t_create_failure)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_create_failure;
					break;
				default:
					g_pLog->myWprintf("[Create_Response_Wait]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CDestroy_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_destroy_success:
					g_pLog->myWprintf("pt_destroy_success\n");
					t_nRemain = sizeof(t_destroy_success)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_destroy_success;
					break;
				case pkt_type::pt_destroy_failure:
					g_pLog->myWprintf("pt_destroy_failure\n");
					t_nRemain = sizeof(t_destroy_failure)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_destroy_failure;
					break;
				}
			}
			else if (CJoin_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_join_success:
					g_pLog->myWprintf("pt_join_success\n");
					t_nRemain = sizeof(t_join_success)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_join_success;
					break;
				case pkt_type::pt_join_failure:
					g_pLog->myWprintf("pt_join_failure\n");
					t_nRemain = sizeof(t_join_failure)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_join_failure;
					break;
				default:
					g_pLog->myWprintf("[CJoin_Response_Wait]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CLeave_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_leave_success:
					g_pLog->myWprintf("pt_leave_success\n");
					t_nRemain = sizeof(t_leave_success)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_leave_success;
					break;
				default:
					g_pLog->myWprintf("[CLeave_Response_Wait]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CRoom::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_join_alarm:
					g_pLog->myWprintf("pt_join_alarm\n");
					t_nRemain = sizeof(t_join_alarm)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_join_alarm;
					break;
				case pkt_type::pt_leave_alarm:
					g_pLog->myWprintf("pt_leave_alarm\n");
					t_nRemain = sizeof(t_leave_alarm)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_leave_alarm;
					break;
				case pkt_type::pt_kick:
					g_pLog->myWprintf("pt_kick\n");
					t_nRemain = sizeof(t_kick)-HEADER_SIZE;
					t_cBot.m_eType = pkt_type::pt_kick;
					break;
					//---------------------------------------------------------------------------------
					// * notice : the character of ASCII whitch number is 12 can't be inputed by keyboard on console !
				case pkt_type::pt_chat_alarm:
					g_pLog->myWprintf("pt_chat_alarm\n");
					t_cBot.m_bIsVar = true;
					t_nRemain = sizeof(unsigned short);
					t_cBot.m_eType = pkt_type::pt_chat_alarm;
					break;
					//---------------------------------------------------------------------------------
				default:
					g_pLog->myWprintf("[Room]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CLobby::Instance() == t_pState)
			{
				switch (t_eType)
				{
				default:
					g_pLog->myWprintf("[Lobby]error : The packet type is default in the ProcEvent of Receiver ! %d\n", t_eType);
				}
			}
			else
			{
				g_pLog->myWprintf("The state of client error in the ProcEvent of the receiver !\n");
			}
		}

		if (0 >= t_nRemain)
		{
			// Is this a variable packet ?
			if (t_cBot.m_bIsVar)
			{
				int typePlusLength = HEADER_SIZE << 1;
				if (t_nPos == typePlusLength){
					memcpy(&t_nRemain, buf + HEADER_SIZE, sizeof(unsigned short));
					t_nRemain -= typePlusLength;
				}
				t_cBot.m_bIsVar = false;
			}
			else
			{
				if (t_cBot.m_eType != pkt_type::pt_chat_alarm)
				{
					t_nRemain = HEADER_SIZE;
					t_nPos = 0;
				}
				else
				{
					t_nRemain = t_nPos;
				}

				// decoding the packet !!
				if (pkt_type::pt_cs_health_check == t_cBot.m_eType)
				{
					t_health_ack msg;
					msg.type = pkt_type::pt_cs_health_ack;
					t_cBot.Send((char*)&msg, sizeof(msg));
				}
				else if (NULL != PacketHandlingFunc[t_cBot.m_eType])
				{
					PacketHandlingFunc[t_cBot.m_eType]((PVOID)p_pAct);

					t_cBot.m_nRecvRemain = HEADER_SIZE;
					t_cBot.m_nRecvPosition = 0;

					if (pkt_type::pt_user_out == t_cBot.m_eType)
					{
						check = false;
					}
				}
				else
				{
					g_pLog->myWprintf("The Packet Handling Func of %d is NULL !\n", t_cBot.m_eType);
				}

				t_cBot.m_eType = pkt_type::pt_default;
			}
		}
	}

	if (check)
		t_cBot.Recv(buf + t_nPos, t_nRemain);
}