#pragma once

struct CClient
{
	CProactor		*m_pProactor;

	CConnector		*m_pConnector;
	CDisconnector	*m_pDisconnector;
	CReceiver		*m_pReceiver;
	CSender			*m_pSender;

	CSockInfo		*m_pSock;
	SOCKET			m_hOldSock;

	int				m_nThreadPoolSize;
	int				m_nSocketPoolSize;

	long long		m_llAccumTime;

	int				m_nTmpRoom_num;
	char			m_szTmpNickname[NICK_SIZE];

	char			m_szNickname[NICK_SIZE];
	int				m_nRoom_num;
	unsigned int	m_nToken;

	bool			m_bIsEscaping;

	//------------------------------------------------------------

	CClient(int p_nThreadPoolSize, int p_nSocketPoolSize = 1);
	~CClient();

	StateMachine<CClient>			*m_pStateMachine;
	StateMachine<CClient>			*GetStateMachine();

	bool SendCreateMessage(int num);
	bool SendDestroyMessage(int num);
	bool SendJoinMessage(int num, char *nick);
	bool SendLeaveMessage();
	bool SendChatMessage(const std::string& str);
	bool SendEscapeSuccessMessage();
	bool SendEscapeFailureMessage();

	bool Init();

	bool SetNickName(char *param)
	{
		if (strlen(param) > 19)
			return false;
		strcpy_s(m_szNickname, param);
		return true;
	}

	bool SetTmpNickName(char *param)
	{
		if (strlen(param) > 19)
			return false;
		strcpy_s(m_szTmpNickname, param);
		return true;
	}
};
