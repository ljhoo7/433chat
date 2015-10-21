#pragma once
#include "stdafx.h"

class AgentApp
{
/// 
///	Singleton Framework Class 
///
private:
	AgentApp();
	AgentApp(const AgentApp&) = delete;
	AgentApp& operator=(const AgentApp&) = delete;
	~AgentApp();

public:
	static AgentApp* Instance()
	{
		static AgentApp instance;

		return &instance;
	}

public:

	void				 Init(DWORD ip, unsigned int port);
	void				 Run();
	void				 Destroy();
						 
public:					 
	/*********************** Set & Get *************************/
	void				 SetAgentNumber(unsigned short agentNumber) { mAgentNumber = agentNumber; }
	unsigned short		 GetAgentNumber() { return mAgentNumber; }
						 
	unsigned int		 GetMServerPort() { return mMonitoringServerPort; }
	DWORD				 GetMServerIP()   { return mMonitoringServerIP; }
						 
	TotalInfo*			 GetTotalInfoData(); 
						 				 
public:					 

	/************** Socket List Managed Function ****************/
	void				 AddServer(SASocket* pSocket);
	void				 DeleteServer(SASocket* pSocket);

public:
	
	/************** Total Data Managed Function ****************/
	void SaveDeltaRoomInfo(unsigned short roomNum, bool isState);
	void SaveDeltaUserInfo(unsigned int serverNum, int clientSocket, unsigned short roomNum, char* nickName, char isConnected);
	void SaveDeltaInterSeverInfo(unsigned short serverNum, bool isConnected);

	void SaveTotalRoomInfo(unsigned short roomCnt, RoomInfo* roomInfoList);
	void SaveTotalServerUserInfo(unsigned int serverNum ,unsigned short userCnt, UserInfo* userInfoList);
	void SaveTotalInterServerInfo(unsigned short serverCnt, unsigned short* serverNumList);

private:
	bool				 mIsExit;
						 
	DWORD				 mMonitoringServerIP;
	unsigned int		 mMonitoringServerPort;
	unsigned short		 mAgentNumber;
						 
						 
	TotalInfo*			 mTotalInfoData;
						 
	ServerAgent*		 mServerAgent;
	MServerAgent*		 mMonitoringServerAgent;

	std::list<SASocket*> mServerSocketList;

	CRITICAL_SECTION	 serverLock;
};
