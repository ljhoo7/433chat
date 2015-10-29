#include "stdafx.h"

extern CLogWriter		*g_pLog;

CProactor::CProactor(DWORD p_dwIp, int p_nPort, int p_nBotCnt, int p_nThreadCnt, int p_nIntervalMin, int p_nIntervalMax)
: m_dwIp(p_dwIp), m_nPort(p_nPort), m_nBotCnt(p_nBotCnt), m_nThreadCnt(p_nThreadCnt), m_nIntervalMin(p_nIntervalMin), m_nIntervalMax(p_nIntervalMax)
{
	int t_nRetval;

	m_hEventForAllJoin = new HANDLE[m_nBotCnt];

	m_pUd = new std::uniform_int_distribution<>(m_nIntervalMin, m_nIntervalMax);
	m_pRandGen = new std::knuth_b();

	//----------------------------------------------------

	SOCKET t_cSock;
	DWORD t_dwBytes;

	/* Dummy socket needed for WSAIoctl */
	t_cSock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == t_cSock)
	{
		g_pLog->myWprintf("I cant' create a dummy socket in CProactor constructor.\n");
		WSACleanup();
		system("pause");
		exit(1);
	}

	GUID t_cGuid = WSAID_CONNECTEX;
	t_nRetval = WSAIoctl(t_cSock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&t_cGuid, sizeof(t_cGuid),
		&m_cMswsock.ConnectEx, sizeof(m_cMswsock.ConnectEx),
		&t_dwBytes, NULL, NULL);

	if (t_nRetval)
	{
		g_pLog->myWprintf("WSAloctl error on CProactor constructor!\n");
		exit(0);
	}

	t_nRetval = closesocket(t_cSock);
	if (t_nRetval)
	{
		g_pLog->myWprintf("closesocket error on main !");
		exit(0);
	}

	//-----------------------------------------

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, m_nThreadCnt);

	if (NULL == m_hIOCP)
	{
		g_pLog->myWprintf("CreateIoCompletionPort function error in CProactor constructor function !!\n");
		system("pause");
		exit(1);
	}
}

CProactor::~CProactor()
{

}

bool CProactor::Init()
{
	int t_nRetval;

	for (int k = 0; k < m_nThreadCnt; ++k)
	{
		unsigned int t_unThreadId;

		t_nRetval = _beginthreadex(NULL, 0, CProactor::ThreadFunc, (void*)this, 0, &t_unThreadId);
		if (-1 == t_nRetval)
		{
			g_pLog->myWprintf("_beginethread error in CProactor Init !\n");
		}
	}

	//-----------------------------------------
	m_pConnector = new CConnector(this);
	m_pDisconnector = new CDisconnector(this);
	m_pReceiver = new CReceiver(this);
	m_pSender = new CSender(this);

	CBot *t_pBot;
	for (int k = 0; k < m_nBotCnt; ++k)
	{
		m_hEventForAllJoin[k] = CreateEvent(NULL, TRUE, FALSE, NULL);

		t_pBot = new CBot(k, k % PLAYER_MAX);
		m_vBot.push_back(t_pBot);
		t_nRetval = t_pBot->Init(m_pConnector, m_pDisconnector, m_pReceiver, m_pSender);
		if (!t_nRetval)
		{
			g_pLog->myWprintf("t_pBot->Init() error in CProactor Init !\n");
		}

		t_nRetval = t_pBot->BindBeforeConnectEx();
		if (!t_nRetval)
		{
			g_pLog->myWprintf("t_pBot->BindBeforeConnectEx error in CProactor Init !\n");
		}

		t_nRetval = Register((HANDLE)t_pBot->m_hNewSock);
		if (!t_nRetval)
		{
			g_pLog->myWprintf("Register error in CProactor Init !\n");
		}

		t_nRetval = t_pBot->Connect(m_cMswsock, m_dwIp, m_nPort);
		if (!t_nRetval)
		{
			g_pLog->myWprintf("t_pBot->Connect in CProactor Init !\n");
		}

		//-------------------------------------------

		int t_nModular = k % PLAYER_MAX;
		int t_nValue = k / PLAYER_MAX;
		if (!t_nModular)
		{
			std::string tmpStr("bot_" + std::to_string(k));
			t_pBot->SetTmpNickName(const_cast<char*>(tmpStr.c_str()));
			t_pBot->m_nTmpRoomNum = t_nValue;
			t_pBot->SendCreateMessage(t_nValue);
		}
		else
		{
			std::string tmpStr("bot_" + std::to_string(k));
			t_pBot->SendJoinMessage(t_nValue, const_cast<char*>(tmpStr.c_str()));
		}
		Sleep(40);
	}

	return true;
}

bool CProactor::Register(HANDLE m_hHandle)
{
	int t_nThreadId = NULL;

	if (NULL == CreateIoCompletionPort(m_hHandle, m_hIOCP, 0, 0))
	{
		g_pLog->myWprintf("CreateIoCompletionPort error in CProactor::Register( ) !\n");

		return false;
	}

	return true;
}

UINT WINAPI CProactor::ThreadFunc(PVOID p_pParam)
{
	CProactor *t_pProactor = static_cast<CProactor*>(p_pParam);

	t_pProactor->ProcEvent();

	return 0;
}

bool CProactor::ProcEvent()
{
	while (true)
	{
		DWORD t_dwBytesTransferred = NULL, t_dwCompletionKey = NULL;

		OVERLAPPED *t_pOverlapped = NULL;

		BOOL t_bStatus;

		t_bStatus = GetQueuedCompletionStatus(m_hIOCP, &t_dwBytesTransferred, static_cast<PULONG_PTR>(&t_dwCompletionKey), &t_pOverlapped, INFINITE);

		if (t_bStatus)
		{
			if (NULL == t_pOverlapped)
			{
				g_pLog->myWprintf("t_pOverlapped is NULL in CProactor::ProcEvent( )!!\n");
				continue;
			}
			CAct *t_pAct = static_cast<CAct*>(t_pOverlapped);
			t_pAct->Complete(t_dwBytesTransferred);
		}
		else
		{
			if (NULL != t_pOverlapped)
			{
				continue;
			}
			g_pLog->myWprintf("t_bStatus (GetQueuedCompletionStatus return value) is false in CProactor::ProcEvent !\n");
			continue;
		}
	}
}