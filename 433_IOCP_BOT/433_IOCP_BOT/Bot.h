#pragma once
class CBot
{
	CProactor		*m_pProactor;

	CBotConnector	*m_pBotConnector;
	CBotReceiver	*m_pBotReceiver;
	CBotSender		*m_pBotSender;

	int				m_nThreadPoolSize;
	int				m_nSocketPoolSize;
public:
	CBot(int p_nThreadPoolSize, int p_nSocketPoolSize);
	~CBot();
};

