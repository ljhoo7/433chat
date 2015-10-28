#pragma once

class CBot
{
public:
	enum ACT_TYPE
	{
		ACT_CONNECT = 0,
		ACT_DISCONNECT,

		ACT_RECEIVE,
		ACT_SEND,
		ACT_CNT
	};

	SOCKET						m_hSock, m_hNewSock;

	bool						m_bIsEscaping;

	StateMachine<CBot>			*m_pStateMachine;
	StateMachine<CBot>			*GetStateMachine();

	int							m_nRoomNum;
	char						m_szNick[NICK_SIZE];

	int							m_nTmpRoomNum;
	char						m_szTmpNick[NICK_SIZE];

	int							m_nToken;

	CAct						*m_vActs[ACT_CNT];

	char						m_szReceiveBuf[BUFSIZE];
	char						m_szSendBuf[BUFSIZE];
	char						m_szConnectBuf[BUFSIZE];

	WSABUF						m_wsaRecvBuf;
	WSABUF						m_wsaSendBuf;

	SOCKADDR_IN					m_cAddr;

	long long					m_llAccumTime;

	int							m_nRecvPosition;
	int							m_nRecvRemain;

	bool						m_bIsVar;
	pkt_type					m_eType;
public:
	CBot()
		: m_bIsEscaping(false), m_nRoomNum(-1), m_nTmpRoomNum(-1), m_nToken(-1), m_hNewSock(NULL)
		, m_nRecvPosition(0), m_nRecvRemain(HEADER_SIZE), m_bIsVar(false), m_eType(pkt_type::pt_default)
	{
		for (int k = 0; k < ACT_CNT; ++k)
			m_vActs[0] = NULL;

		// Initialize The State Machine
		m_pStateMachine = new StateMachine<CBot>(this);
		GetStateMachine()->SetCurrentState(CLobby::Instance());
	}
	~CBot(){}

	bool Init(CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver, CSender *p_pSender);
	bool BindBeforeConnectEx();
	bool Connect(mswsock_s &p_cConnEx, DWORD p_dwIp, int p_nPort);
	bool Disconnect();

	bool SetNickName(char *param)
	{
		if (strlen(param) > 19)
			return false;
		strcpy_s(m_szNick, param);
		return true;
	}

	bool SetTmpNickName(char *param)
	{
		if (strlen(param) > 19)
			return false;
		strcpy_s(m_szTmpNick, param);
		return true;
	}

	bool Recv(char *p_pBuf, int p_nBufSize);
	bool Send(char *p_pBuf, int p_nBufLen);

	bool SendCreateMessage(int p_nNum);
	bool SendDestroyMessage(int p_nNum);
	bool SendJoinMessage(int p_nNum, char *p_szNick);
	bool SendLeaveMessage();
	bool SendChatMessage(const std::string& p_sStr);
	bool SendEscapeSuccessMessage();
	bool SendEscapeFailureMessage();
};

