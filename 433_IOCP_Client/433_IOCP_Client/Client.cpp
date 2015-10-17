#include "stdafx.h"

CClient::CClient(int p_nThreadPoolSize, int p_nSocketPoolSize)
:m_nThreadPoolSize(p_nThreadPoolSize), m_nSocketPoolSize(p_nSocketPoolSize)
{
	m_pProactor = new CProactor(p_nThreadPoolSize);

	m_pConnector = new CConnector(m_pProactor);
	m_pDisconnector = new CDisconnector(m_pProactor);
	m_pReceiver = new CReceiver(m_pProactor);
	m_pSender = new CSender(m_pProactor);

	//m_pProactor

	for (int k = 0; k < p_nSocketPoolSize; ++k)
	{
		CSockInfo *sock = new CSockInfo(m_pProactor, m_pConnector, m_pDisconnector, m_pReceiver, m_pSender);

		m_pConnector->Register(*sock);
	}
}


CClient::~CClient()
{
}
