#pragma once

#include "Common.h"


class MServerAgent
{
public:
	CProactor						*m_pProactor;
	CConnector						*m_pConnector;
	CDisconnector					*m_pDisconnector;
	CReceiver						*m_pReceiver;
	CSender							*m_pSender;

	TcpSocket						*m_pSock;
	TcpSocket						*m_pServerAgentSocket;

	int								m_nThreadPoolSize;
	int								m_nSocketPoolSize;

	int								m_nPosition;
	int								m_nRemainBytes;


	bool							isServerAgentConnected;
	bool							isUse;
	bool							isVar;

	long long						m_llAccumTime;

	TotalInfo*						m_pTotalInfoData;

	CRITICAL_SECTION		        IOLock;

public:
	MServerAgent(TotalInfo* totalInfoData, int p_nThreadPoolSize, int p_nSocketPoolSize = 1);
	~MServerAgent();

	void RecvProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void SendProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, CAct* act, DWORD bytes_transferred);

	void PacketHandling(char* buf);

	void SendTotalData();
	void SendUserInfo();
	void SendRoomInfo();
	void SendInterServerInfo();

	void Connect(DWORD ip, WORD port);

public:
	void IsConnected(ServerAgent* pServerAgent);
};

