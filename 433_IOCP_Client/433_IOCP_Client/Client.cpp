#include "stdafx.h"

extern CLogWriter *g_pLog;
extern DWORD g_dwIp;
extern int t_nPort;

CClient::CClient(int p_nThreadPoolSize, int p_nSocketPoolSize)
:m_nThreadPoolSize(p_nThreadPoolSize), m_nSocketPoolSize(p_nSocketPoolSize)
, m_nRoom_num(-1), m_nTmpRoom_num(-1), m_hNewSock(NULL)
{
	strcpy(m_szNickname, "");
	strcpy(m_szTmpNickname, "");

	// Initialize The State Machine
	m_pStateMachine = new StateMachine<CClient>(this);
	GetStateMachine()->SetCurrentState(CLobby::Instance());
}

bool CClient::Init()
{
	m_pProactor = new CProactor(m_nThreadPoolSize);

	m_pConnector = new CConnector(m_pProactor);
	m_pDisconnector = new CDisconnector(m_pProactor);
	m_pReceiver = new CReceiver(m_pProactor);
	m_pSender = new CSender(m_pProactor);

	m_pSock = new CSockInfo(m_pProactor, m_pConnector, m_pDisconnector, m_pReceiver, m_pSender);

	m_pSock->Connect(g_dwIp, t_nPort);

	return true;
}

CClient::~CClient()
{
}

StateMachine<CClient> *CClient::GetStateMachine()
{
	if (m_pStateMachine == NULL)
	{
		g_pLog->myWprintf(L"Error : m_pStateMachine is nullptr !\n");

		return NULL;
	}
	else
	{
		return m_pStateMachine;
	}
}