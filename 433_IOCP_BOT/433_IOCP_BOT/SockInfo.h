#pragma once

struct CSockInfo
{
	SOCKET			m_hSock;

	TCHAR			m_szReciveBuf[BUFSIZE];
	TCHAR			m_szSendBuf[BUFSIZE];
	TCHAR			m_szConnectBuf[BUFSIZE];

	WSABUF			m_wsaRecvBuf;
	WSABUF			m_wsaSendBuf;

	CProactor		*m_pProactor;
	
	CBotConnector	*m_pBotConnector;
	CBotReceiver	*m_pBotReceiver;
	CBotSender		*m_pBotSender;

	SOCKADDR_IN m_cAddr;

	enum ACT_TYPE
	{
		ACT_BOTCONNECT = 0,
		ACT_BOTRECV,
		ACT_BOTSEND,
		ACT_TYPE_CNT
	};

	std::vector<CAct*>		m_vpAct;

	CSockInfo() {}

	CSockInfo(CProactor *p_pProactor, CBotConnector *p_pBotConnector, CBotReceiver *p_pBotReceiver
		, CBotSender *p_pBotSender);
	~CSockInfo();
};