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
	
	CConnector		*m_pConnector;
	CDisconnector	*m_pDisconnector;
	CReceiver		*m_pReceiver;
	CSender			*m_pSender;

	SOCKADDR_IN m_cAddr;

	enum ACT_TYPE
	{
		ACT_CONNECT = 0,
		ACT_DISCONNECT,
		ACT_RECV,
		ACT_SEND,
		ACT_TYPE_CNT
	};

	std::vector<CAct*>		m_vpAct;

	CSockInfo() {}

	CSockInfo(CProactor *p_pProactor, CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver, CSender *p_pSender);
	~CSockInfo();
};