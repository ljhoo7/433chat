#pragma once

#include "Common.h"


class MServerAgent : public WinSockBase
{
public:
	Proactor*						mProactor;
	Connector*						mConnector;
	Disconnector*					mDisconnector;
	Receiver*						mReceiver;
	Sender*							mSender;

	TcpSocket*					    m_pSock;

	int								m_nThreadPoolSize;
	int								m_nSocketPoolSize;

	int								m_nPosition;
	int								m_nRemainBytes;

	bool							isUse;
	bool							isVar;

	long long						m_llAccumTime;


	CRITICAL_SECTION		        IOLock;

public:
	MServerAgent(int p_nThreadPoolSize, int p_nSocketPoolSize = 1);
	virtual ~MServerAgent();

	void RecvProcess(bool isError, Act* act, DWORD bytes_transferred);
	void SendProcess(bool isError, Act* act, DWORD bytes_transferred);
	void DisconnProcess(bool isError, Act* act, DWORD bytes_transferred);
	void ConnProcess(bool isError, Act* act, DWORD bytes_transferred);

	void PacketHandling(char* buf);

	void SendUserOut(int serverNum, int userSocket);
	void SendRoomDestroy(int roomNum);
	void SendGenerateServer();
	void SendKillServer(int serverNum);

	void SendTotalData();
	void SendServerCount();
	void SendServerInfo();
	void SendRoomInfo();

	void Connect(DWORD ip, WORD port);

};

