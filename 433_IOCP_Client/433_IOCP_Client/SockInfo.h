#pragma once

struct CSockInfo
{
	SOCKET			m_hSock, m_hOtherSock;

	bool			m_bIsVar;
	bool			m_bWhich;

	int				m_nRecvPosition;
	int				m_nRecvRemain;

	char			m_szReceiveBuf[BUFSIZE];
	char			m_szSendBuf[BUFSIZE];
	char			m_szConnectBuf[BUFSIZE];

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

	CAct			*m_vAct[ACT_TYPE_CNT];

	CSockInfo() : m_nRecvPosition(0), m_bIsVar(false), m_bWhich(true)
	{
		m_vAct[0] = NULL;
		m_vAct[1] = NULL;
		m_vAct[2] = NULL;
		m_vAct[3] = NULL;
	}

	CSockInfo(CProactor *p_pProactor, CConnector *p_pConnector, CDisconnector *p_pDisconnector, CReceiver *p_pReceiver, CSender *p_pSender);
	~CSockInfo();

	bool Recv(char *p_pBuf, int p_nBufSize);
	bool Send(char *p_pBuf, int p_nBufLen);
	bool Connect(DWORD ip, int port);
	bool Disconnect();

	void BindBeforeConnectEx();

	/*bool ReceiveProcess(CAct *act, DWORD bytes_transferred);
	bool SendProcess(CAct *act, DWORD bytes_transferred);
	bool AcceptProcess(CAct *act, DWORD bytes_transferred);
	bool DisconnectProcess(CAct *act, DWORD bytes_transferred);*/
};