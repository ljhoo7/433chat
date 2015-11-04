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

	void				   Init(unsigned short agentPort, DWORD ip, unsigned int port);
	void				   Run();
	void				   Destroy();
	
	bool				   IsExit() { return mIsExit; }

public:
	/**************** Control Server Process *******************/
	bool				   GenerateServerProcess();
	void				   GenerateTimeWait(int serverNum);

public:					 
	/*********************** Set & Get *************************/
	void				   SetAgentNumber(unsigned short agentNumber) { mAgentNumber = agentNumber; }
	unsigned short		   GetAgentNumber() { return mAgentNumber; }
						   
	unsigned int		   GetMServerPort() { return mMonitoringServerPort; }
	DWORD				   GetMServerIP()   { return mMonitoringServerIP; }
						   
	

public:
	bool				   IsProcessConnected(int serverNum);
	const HANDLE&		   GetProcessInfoHandle(int serverNum);
	void				   SetProcessConnected(int serverNum, bool connected);
	void				   AddProcessInfo(int serverNum, bool connected, const HANDLE& hProcess);
	void				   DeleteProcessInfo(int serverNum);

public:					 
	/************** Socket List Managed Function ****************/
	void				   AddServer(SASocket* pSocket);
	void				   DeleteServer(SASocket* pSocket);
	SASocket*			   FindServer(int serverNum);
	
	int					   GetServerCount() { return mServerSocketList.size(); }
	std::list<SASocket*>&  GetServerSocketList() { return mServerSocketList; }

public:
	/************** Total Data Managed Function ****************/
	void				   SaveDeltaRoomInfo(unsigned short roomNum, bool isState);
	void				   SaveDeltaUserInfo(unsigned int serverNum, int clientSocket, unsigned short roomNum, char* nickName, char isConnected);
						   
	void				   SaveTotalRoomInfo(unsigned short roomCnt, RoomInfo* roomInfoList);
	void				   SaveTotalServerUserInfo(unsigned int serverNum ,unsigned short userCnt, UserInfo* userInfoList);
						   
	bool				   DeleteServerInfo(int serverNum);

	bool				   IsSearchUser(int serverNum, int userSocket);
	bool				   IsSearchRoom(unsigned short roomNum);
	
	TotalInfo*			   GetTotalInfoData(); 

private:
	bool				     mIsExit;
			
	DWORD				     mMonitoringServerIP;
	unsigned int		     mMonitoringServerPort;
	unsigned short		     mAgentNumber;
						     
	TotalInfo*			     mTotalInfoData;
						     
	ServerAgent*		     mServerAgent;
	MServerAgent*		     mMonitoringServerAgent;
						   
	std::list<SASocket*>     mServerSocketList;

	std::vector<std::thread> mTimeWaitThreads;

	std::list<ProcessInfo>   mProcessCheckList;
						   
	CRITICAL_SECTION	     serverLock;
};
