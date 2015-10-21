#pragma once

#include "Common.h"


class ServerAgent
{
public:
	CProactor						*m_pProactor;
	CConnector						*m_pConnector;
	CDisconnector					*m_pDisconnector;
	CReceiver						*m_pReceiver;
	CSender							*m_pSender;

	TcpSocket						*m_pSock;
	
	int								m_nThreadPoolSize;
	int								m_nSocketPoolSize;
	
	int								m_nPosition;
	int								m_nRemainBytes;


	bool							isConnect;
	bool							isUse;
	bool							isVar;

	long long						m_llAccumTime;

	TotalInfo*						m_pTotalInfoData;

	CRITICAL_SECTION		        IOLock;

public:
	ServerAgent(TotalInfo* totalInfoData, int p_nThreadPoolSize, int p_nSocketPoolSize = 1);
	~ServerAgent();

	void RecvProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void SendProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, CAct* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, CAct* act, DWORD bytes_transferred);

	void PacketHandling(char* buf);

	void SaveAgentNumber(unsigned short agentNum);
	
	void SaveDeltaRoomInfo(unsigned short roomNum, bool isState);
	void SaveDeltaUserInfo(int clientSocket, unsigned short roomNum, char* nickName, char isConnected);
	void SaveDeltaInterSeverInfo(unsigned short serverNum, bool isConnected);

	void SaveTotalRoomInfo(unsigned short roomCnt, RoomInfo* roomInfoList);
	void SaveTotalUserInfo(unsigned short userCnt, UserInfo* userInfoList);
	void SaveTotalInterServerInfo(unsigned short serverCnt, unsigned short* serverNumList);

	void Connect(DWORD ip, WORD port);
};

