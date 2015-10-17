#pragma once

class CClient
{
	CProactor		*m_pProactor;

	CConnector		*m_pConnector;
	CDisconnector	*m_pDisconnector;
	CReceiver		*m_pReceiver;
	CSender			*m_pSender;

	int				m_nThreadPoolSize;
	int				m_nSocketPoolSize;
public:
	CClient(int p_nThreadPoolSize, int p_nSocketPoolSize = 1);
	~CClient();
};

