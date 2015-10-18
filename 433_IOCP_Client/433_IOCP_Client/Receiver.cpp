#include "stdafx.h"

extern CLogWriter	*g_pLog;
extern CClient		*g_pClient;

bool CreateSuccessFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool CreateFailureFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool DestroySuccessFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool DestroyFailureFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool JoinSuccessFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool JoinFailureFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool LeaveSuccessFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool ChatAlarmFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool JoinAlarmFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool LeaveAlarmFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool KickFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool UserOutFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool EscapeSuccessFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

bool EscapeFailureFunc(PVOID p_pParam)
{
	CSockInfo *t_pSock = (CSockInfo*)p_pParam;

	return true;
}

CReceiver::CReceiver(CProactor *p_pProactor)
{
	m_pProactor = p_pProactor;

	int k = 0;

	// create
	PacketHandlingFunc[k++] = NULL;

	// create_success
	PacketHandlingFunc[k++] = CreateSuccessFunc;

	// create_failure
	PacketHandlingFunc[k++] = CreateFailureFunc;

	// destroy
	PacketHandlingFunc[k++] = NULL;

	// destroy success
	PacketHandlingFunc[k++] = DestroySuccessFunc;

	// destroy failure
	PacketHandlingFunc[k++] = DestroyFailureFunc;

	// join
	PacketHandlingFunc[k++] = NULL;

	// join success
	PacketHandlingFunc[k++] = JoinSuccessFunc;

	// join failure
	PacketHandlingFunc[k++] = JoinFailureFunc;

	// leave
	PacketHandlingFunc[k++] = NULL;

	// leave success
	PacketHandlingFunc[k++] = LeaveSuccessFunc;

	// chat
	PacketHandlingFunc[k++] = NULL;

	// chat alarm
	PacketHandlingFunc[k++] = ChatAlarmFunc;

	// join alarm
	PacketHandlingFunc[k++] = JoinAlarmFunc;

	// leave alarm
	PacketHandlingFunc[k++] = LeaveAlarmFunc;

	// kick
	PacketHandlingFunc[k++] = KickFunc;

	// user out
	PacketHandlingFunc[k++] = UserOutFunc;

	// escape success
	PacketHandlingFunc[k++] = EscapeSuccessFunc;

	// escape failure
	PacketHandlingFunc[k++] = EscapeFailureFunc;

	if (k != pkt_type::pt_default)
	{
		g_pLog->myWprintf(L"[error] The number of packet handling functions is not matched with the number of packet.\n");
	}
}


CReceiver::~CReceiver()
{
}

void CReceiver::ProcEvent(CAct *p_pAct, DWORD p_dwTransferredBytes)
{
	CSockInfo &t_hSock = *p_pAct->m_pSock;
	if (0 == p_dwTransferredBytes)
	{
		t_hSock.Disconnect();
		return;
	}

	int &t_nPos = t_hSock.m_nRecvPosition;
	t_nPos += p_dwTransferredBytes;
	int &t_nRemain = t_hSock.m_nRecvRemain;
	t_nRemain -= p_dwTransferredBytes;

	char *buf = t_hSock.m_szReceiveBuf;

	if (HEADER_SIZE <= t_nPos)
	{
		if (HEADER_SIZE == t_nPos)
		{
			pkt_type t_eType = (pkt_type)((unsigned short)(*buf));
			
			State<CClient> *t_pState = g_pClient->GetStateMachine()->CurrentState();

			if (pkt_type::pt_user_out == t_eType)
			{
				g_pLog->myWprintf(L"pt_user_out\n");
				t_nRemain = sizeof(t_user_out) - HEADER_SIZE;
				p_pAct->m_eType = pkt_type::pt_user_out;
			}
			else if (pkt_type::pt_escape_server == t_eType)
			{
				g_pLog->myWprintf(L"pt_escape_server\n");
				t_nRemain = sizeof(t_escape_server) - HEADER_SIZE;
				p_pAct->m_eType = pkt_type::pt_escape_server;
			}
			else if (CCreate_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_create_success:
					g_pLog->myWprintf(L"pt_create_success\n");
					t_nRemain = sizeof(t_create_success)-HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_create_success;
					break;
				case pkt_type::pt_create_failure:
					g_pLog->myWprintf(L"pt_create_failure\n");
					t_nRemain = sizeof(t_create_failure)-HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_create_failure;
					break;
				default:
					g_pLog->myWprintf(L"[Create_Response_Wait]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CDestroy_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_destroy_success:
					g_pLog->myWprintf(L"pt_destroy_success\n");
					t_nRemain = sizeof(t_destroy_success)-HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_destroy_success;
					break;
				case pkt_type::pt_destroy_failure:
					g_pLog->myWprintf(L"pt_destroy_failure\n");
					t_nRemain = sizeof(t_destroy_failure)-HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_destroy_failure;
					break;
				}
			}
			else if (CJoin_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_join_success:
					g_pLog->myWprintf(L"pt_join_success\n");
					t_nRemain = sizeof(t_join_success)-HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_join_success;
					break;
				case pkt_type::pt_join_failure:
					g_pLog->myWprintf(L"pt_join_failure\n");
					t_nRemain = sizeof(t_join_failure)-HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_join_failure;
					break;
				default:
					g_pLog->myWprintf(L"[CJoin_Response_Wait]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CLeave_Response_Wait::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_leave_success:
					g_pLog->myWprintf(L"pt_leave_success\n");
					t_nRemain = sizeof(t_leave_success)-HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_leave_success;
					break;
				default:
					g_pLog->myWprintf(L"[CLeave_Response_Wait]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CRoom::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_join_alarm:
					g_pLog->myWprintf(L"pt_join_alarm\n");
					t_nRemain = sizeof(t_chat_alarm) - HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_chat_alarm;
					break;
				case pkt_type::pt_leave_alarm:
					g_pLog->myWprintf(L"pt_leave_alarm\n");
					t_nRemain = sizeof(t_leave_alarm) - HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_leave_alarm;
					break;
				case pkt_type::pt_kick:
					g_pLog->myWprintf(L"pt_kick\n");
					t_nRemain = sizeof(t_kick) - HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_kick;
					break;
//---------------------------------------------------------------------------------
				case pkt_type::pt_chat:
					g_pLog->myWprintf(L"pt_chat\n");
					t_hSock.m_bIsVar = true;
					t_nRemain = sizeof(unsigned short);
					p_pAct->m_eType = pkt_type::pt_chat;
					break;
				case pkt_type::pt_chat_alarm:
					g_pLog->myWprintf(L"pt_chat_alarm\n");
					t_hSock.m_bIsVar = true;
					t_nRemain = sizeof(unsigned short);
					p_pAct->m_eType = pkt_type::pt_chat_alarm;
					break;
//---------------------------------------------------------------------------------
				default:
					g_pLog->myWprintf(L"[Room]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CLobby::Instance() == t_pState)
			{
				switch (t_eType)
				{
				default:
					g_pLog->myWprintf(L"[Lobby]error : The packet type is default in the ProcEvent of Receiver !");
				}
			}
			else if (CEscaping::Instance() == t_pState)
			{
				switch (t_eType)
				{
				case pkt_type::pt_escape_success:
					g_pLog->myWprintf(L"pt_escape_server_success\n");
					t_nRemain = sizeof(t_escape_success) - HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_escape_success;
					break;
				case pkt_type::pt_escape_failure:
					g_pLog->myWprintf(L"pt_escape_server_failure\n");
					t_nRemain = sizeof(t_escape_failure) - HEADER_SIZE;
					p_pAct->m_eType = pkt_type::pt_escape_failure;
					break;
				}
			}
			else
			{
				g_pLog->myWprintf(L"The state of client error in the ProcEvent of the receiver !\n");
			}
		}

		if (0 >= t_nRemain)
		{
			// Is this a variable packet ?
			if (t_hSock.m_bIsVar)
			{
				int typePlusLength = HEADER_SIZE << 1;
				if (t_nPos == typePlusLength){
					memcpy(&t_nRemain, buf + HEADER_SIZE, sizeof(short));
					t_nRemain -= typePlusLength;
				}
				t_hSock.m_bIsVar = false;
			}
			else
			{
				t_nPos = 0;
				
				// decoding the packet !!
				if (NULL != PacketHandlingFunc[p_pAct->m_eType])
				{
					PacketHandlingFunc[p_pAct->m_eType]((PVOID)&t_hSock);
				}
				else
				{
					g_pLog->myWprintf(L"The Packet Handling Func of %d is NULL !\n", p_pAct->m_eType);
				}

				p_pAct->m_eType = pkt_type::pt_default;
			}
		}
	}
	t_hSock.Recv(buf + t_nPos, t_nRemain);
}
void CReceiver::ProcError(CAct *p_pAct, DWORD p_dwError)
{

}
