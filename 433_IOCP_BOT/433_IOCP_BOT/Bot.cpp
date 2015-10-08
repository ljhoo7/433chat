#include "stdafx.h"

CBot::CBot(int p_nThreadPoolSize, int p_nSocketPoolSize)
:m_nThreadPoolSize(p_nThreadPoolSize), m_nSocketPoolSize(p_nSocketPoolSize)
{
	printf("b1\n");
	m_pProactor = new CProactor(p_nThreadPoolSize);

	m_pBotConnector = new CBotConnector(m_pProactor);
	m_pBotReceiver = new CBotReceiver(m_pProactor);
	m_pBotSender = new CBotSender(m_pProactor);

	for (int k = 0; k < p_nSocketPoolSize; ++k)
	{
		CSockInfo *sock = new CSockInfo(m_pProactor, m_pBotConnector, m_pBotReceiver, m_pBotSender);

		m_pBotConnector->Register(*sock);
	}
	printf("b2\n");
}


CBot::~CBot()
{
}
