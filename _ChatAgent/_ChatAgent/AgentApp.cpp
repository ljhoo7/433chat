#include "stdafx.h"
#include "AgentApp.h"


AgentApp::AgentApp()
:mMonitoringServerIP(0),
 mMonitoringServerPort(-1),
 mAgentNumber(0),
 mIsExit(false)
{
	mTotalInfoData		   = nullptr;
	mServerAgent		   = nullptr;
	mMonitoringServerAgent = nullptr;

	InitializeCriticalSectionAndSpinCount(&serverLock, 4000);
}


AgentApp::~AgentApp()
{
	DeleteCriticalSection(&serverLock);
}

void AgentApp::Init(unsigned short agentPort, DWORD ip, unsigned int port)
{
	mMonitoringServerIP   = ip;
	mMonitoringServerPort = port;

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	int nCPUs = (int)si.dwNumberOfProcessors;

	mTotalInfoData			= new TotalInfo;
	mServerAgent			= new ServerAgent(agentPort, nCPUs * 2, 100);
	mMonitoringServerAgent  = new MServerAgent(nCPUs * 2, 1);

	mServerAgent->Run();
}

void AgentApp::Destroy()
{
	/// ���� �����ʹ� �����ӿ�ũ Ŭ�������� �Ҹ�
	if (mTotalInfoData)			delete mTotalInfoData;
	if (mServerAgent)			delete mServerAgent;
	if (mMonitoringServerAgent) delete mMonitoringServerAgent;
}

void AgentApp::Run()
{
	PRINTF("Input Command...\n");

	std::string input;
	std::getline(std::cin, input);

	if ( input == "connect" )
	{
		PRINTF("Request Connecting Monitoring Server \n");
		
		mMonitoringServerAgent->Connect(mMonitoringServerIP, mMonitoringServerPort);

	}
	else if ( input == "show" )
	{
		PRINTF("---------------------------------------------------------------------\n");
		PRINTF("----------------------- Total Server Information --------------------\n");
		PRINTF("---------------------------------------------------------------------\n");
		PRINTF("---------------------------- Room Information -----------------------\n");
		PRINTF("Room Count : %d \n", mTotalInfoData->roomCnt);
		PRINTF("Room List\n");
		for (auto& room : mTotalInfoData->roomInfoList)
		{
			PRINTF("%d\t", room.roomNum);
		}
		PRINTF("\n");
		
		PRINTF("--------------------------- Server Information ----------------------\n");
		for (auto& serverInfo : mTotalInfoData->serverUserInfoList)
		{
			PRINTF("Server Num : %d\n", serverInfo.serverNum);
			PRINTF("In Server User Count : %d\n", serverInfo.userCount);
			PRINTF("In Server User Info List\n");
			for (auto& user : serverInfo.userInfoList)
			{
				PRINTF("User Name : %s\n", user.userName);
				PRINTF("User Socket : %d\n", user.userSocket);
				PRINTF("User Room Number : %d\n", user.roomNum);
				PRINTF("\n");

			}
			PRINTF("\n");
		}
		PRINTF("\n");

	/*	PRINTF("------------------------ InterServer Information --------------------\n");
		PRINTF("Server Count : %d\n", mTotalInfoData->serverCnt);
		PRINTF("[Server Number List]\n");
		for (auto& serverNum : mTotalInfoData->serverNumList)
		{
			PRINTF("%d\t", serverNum);
		}*/
		PRINTF("\n\n");

	}
	else if (input == "ms-disconnect")
	{

	}
	else if (input == "quit")
	{


	}

}

TotalInfo* AgentApp::GetTotalInfoData()
{
	if (mTotalInfoData) return mTotalInfoData;
	else				return nullptr;
}

void AgentApp::AddServer(SASocket* pSocket)
{
	EnterCriticalSection(&serverLock);

	mServerSocketList.push_back(pSocket);

	ServerInfo serverInfo;
	serverInfo.serverNum = pSocket->mServerNum;
	mTotalInfoData->serverUserInfoList.push_back(serverInfo);

	LeaveCriticalSection(&serverLock);
}

void AgentApp::DeleteServer(SASocket* pSocket)
{
	EnterCriticalSection(&serverLock);
	mServerSocketList.remove(pSocket);
	LeaveCriticalSection(&serverLock);
}
SASocket* AgentApp::FindServer(int serverNum)
{
	EnterCriticalSection(&serverLock);
	std::list<SASocket*>::iterator iter;
	for (iter = mServerSocketList.begin(); iter != mServerSocketList.end(); iter++)
	{
		SASocket* pSocket = *(iter);
		/// �ϴ� �����ѹ��θ� �Ǵ�
		if (pSocket->mServerNum == serverNum)
		{
			LeaveCriticalSection(&serverLock);
			return pSocket;
		}
	}

	LeaveCriticalSection(&serverLock);
	return NULL;
}



void AgentApp::SaveDeltaRoomInfo(unsigned short roomNum, bool isState)
{
	EnterCriticalSection(&serverLock);

	if (isState == 1)
	{
		/// Created
		mTotalInfoData->roomCnt++;

		RoomInfo room;
		room.roomNum = roomNum;
		mTotalInfoData->roomInfoList.push_back(room);

	}
	else
	{
		/// Destroyed
		if (mTotalInfoData->roomCnt == 0)
		{
			PRINTF("ERROR : already room count is zero");
		}
		else
		{
			mTotalInfoData->roomCnt--;
			mTotalInfoData->roomInfoList.remove_if([&](RoomInfo& room){
				return room.roomNum == roomNum;
			});
		}
	}

	LeaveCriticalSection(&serverLock);
}

void AgentApp::SaveDeltaUserInfo(unsigned int serverNum, int clientSocket, unsigned short roomNum, char* nickName, char isConnected)
{
	EnterCriticalSection(&serverLock);

	std::list<ServerInfo>& serverInfoList = mTotalInfoData->serverUserInfoList;

	auto serverInfoIter = std::find_if(serverInfoList.begin(), 
		serverInfoList.end(), 
		[&](ServerInfo& serverInfo){
		return serverInfo.serverNum == serverNum;
	});

	if (serverInfoIter == serverInfoList.end())
	{
		PRINTF("ERROR : Not Exist Server Number \n");
		
		LeaveCriticalSection(&serverLock);

		return;
	}
	
	if (isConnected == 0)
	{
		/// Disconnected
		if (serverInfoIter->userCount == 0)
		{
			PRINTF("ERROR : already user count is zero \n");
		}
		else
		{
			PRINTF("User Disconnect! \n ");
			serverInfoIter->userCount--;
			serverInfoIter->userInfoList.remove_if([&](UserInfo& user){
				return (user.roomNum == roomNum) &&
					(user.userSocket == clientSocket);
			
			});
		}

	}
	else if (isConnected == 1)
	{
		// Connected
		PRINTF("User Connect! \n ");
		serverInfoIter->userCount++;

		UserInfo user;
		user.userSocket = clientSocket;
		user.roomNum = roomNum;
		strcpy_s(user.userName, NICK_SIZE, nickName);

		serverInfoIter->userInfoList.push_back(user);

	}
	else if (isConnected == 2)
	{
		// Join
		PRINTF("User Join! \n ");
		auto userIter = std::find_if(serverInfoIter->userInfoList.begin(),
			serverInfoIter->userInfoList.end(),
			[&](UserInfo& user) {
			return user.userSocket == clientSocket;
		});

		userIter->roomNum = roomNum;
		memcpy(userIter->userName, nickName, NICK_SIZE);

	}
	else if (isConnected == 3)
	{
		// Leave
		PRINTF("User Leave! \n ");

		auto userIter = std::find_if(serverInfoIter->userInfoList.begin(),
			serverInfoIter->userInfoList.end(),
			[&](UserInfo& user) {
			return user.userSocket == clientSocket;
		});

		userIter->roomNum = roomNum;
		memcpy(userIter->userName, nickName, NICK_SIZE);

	}
	else
	{
		PRINTF("ERROR : NOT VALID\n");
	}

	LeaveCriticalSection(&serverLock);
}

void AgentApp::SaveDeltaInterSeverInfo(unsigned short serverNum, bool isConnected)
{
	EnterCriticalSection(&serverLock);

	if (isConnected == 0)
	{
		/// Disconnected
		if (mTotalInfoData->serverCnt == 0)
		{
			PRINTF("ERROR : already server count is zero\n");
		}
		else
		{
			mTotalInfoData->serverCnt--;
			mTotalInfoData->serverNumList.remove(serverNum);
		}
	}
	else if (isConnected == 1)
	{
		
		// Connected
		mTotalInfoData->serverCnt++;
		mTotalInfoData->serverNumList.push_back(serverNum);

	}


	LeaveCriticalSection(&serverLock);
}
void AgentApp::SaveTotalRoomInfo(unsigned short roomCnt, RoomInfo* roomInfoList)
{
	EnterCriticalSection(&serverLock);

	
	if (mTotalInfoData->roomCnt == roomCnt)
	{
		LeaveCriticalSection(&serverLock);
		return;
	}
	else
	{
		if (!mTotalInfoData->roomInfoList.empty())
			mTotalInfoData->roomInfoList.clear();

		mTotalInfoData->roomCnt = roomCnt;

		for (int i = 0; i < roomCnt; ++i)
		{

			mTotalInfoData->roomInfoList.push_back(roomInfoList[i]);

		}
	}
	

	LeaveCriticalSection(&serverLock);

}

void AgentApp::SaveTotalServerUserInfo(unsigned int serverNum, unsigned short userCnt, UserInfo* userInfoList)
{
	EnterCriticalSection(&serverLock);


	/*if (!mTotalInfoData->serverUserInfoList.empty())
	{
		mTotalInfoData->serverUserInfoList.clear();
	}*/

	bool isExist = false;
	for (auto& serverInfo : mTotalInfoData->serverUserInfoList)
	{
		if ((serverInfo.serverNum == serverNum))
		{
			isExist = true;
			if (serverInfo.userCount != userCnt)
			{
				serverInfo.userInfoList.clear();
				serverInfo.userCount = userCnt;
				for (int i = 0; i < userCnt; ++i)
					serverInfo.userInfoList.push_back(userInfoList[i]);

				
			}
			break;
		}
	}
	if (!isExist)
	{
		ServerInfo serverInfo;
		serverInfo.serverNum = serverNum;
		serverInfo.userCount = userCnt;
		for (int i = 0; i < userCnt; ++i)
			serverInfo.userInfoList.push_back(userInfoList[i]);
	}

	LeaveCriticalSection(&serverLock);
}

void AgentApp::SaveTotalInterServerInfo(unsigned short serverCnt, unsigned short* serverNumList)
{
	EnterCriticalSection(&serverLock);

	if (mTotalInfoData->serverCnt == serverCnt)
	{
		LeaveCriticalSection(&serverLock);
		return;
	}
	else
	{
		if (!mTotalInfoData->serverNumList.empty())
			mTotalInfoData->serverNumList.clear();

		mTotalInfoData->serverCnt = serverCnt;

		for (int i = 0; i < serverCnt; ++i)
		{

			mTotalInfoData->serverNumList.push_back(serverNumList[i]);

		}
	}

	LeaveCriticalSection(&serverLock);
}

bool AgentApp::DeleteServerInfo(int serverNum)
{
	EnterCriticalSection(&serverLock);

	std::list<ServerInfo>& serverInfoList = mTotalInfoData->serverUserInfoList;

	auto findServerIter = std::find_if(serverInfoList.begin(),
		serverInfoList.end(),
		[&](ServerInfo& serverInfo){ return serverInfo.serverNum == serverNum; });

	if (findServerIter == serverInfoList.end())
	{
		PRINTF("Failed Search Server Number \n");
		LeaveCriticalSection(&serverLock);
		return false;
	}
	
	PRINTF("Success Delete Server \n");
	serverInfoList.remove((*findServerIter));

	LeaveCriticalSection(&serverLock);

	return true;
}

bool AgentApp::IsSearchUser(int serverNum, int userSocket)
{
	EnterCriticalSection(&serverLock);

	std::list<ServerInfo>& serverInfoList = mTotalInfoData->serverUserInfoList;

	auto findServerIter = std::find_if(serverInfoList.begin(), 
									serverInfoList.end(),
									[&](ServerInfo& serverInfo){ return serverInfo.serverNum == serverNum; });
	if (findServerIter == serverInfoList.end())
	{
		PRINTF("Failed Search Server Number \n");
		LeaveCriticalSection(&serverLock);
		return false;
	}
	
	for (auto& userInfo : findServerIter->userInfoList)
	{
		if (userInfo.userSocket == userSocket)
		{
			PRINTF("Complete Search User Socket\n");
			LeaveCriticalSection(&serverLock);
			return true;
		}
	}
	
	PRINTF("Failed Search User Socket\n");
	LeaveCriticalSection(&serverLock);
	return false;

	
}

bool AgentApp::IsSearchRoom(unsigned short roomNum)
{
	EnterCriticalSection(&serverLock);

	std::list<RoomInfo>& roomInfoList = mTotalInfoData->roomInfoList;
	
	auto findServerIter = std::find_if(roomInfoList.begin(),
		roomInfoList.end(),
		[&](RoomInfo& roomInfo){ return roomInfo.roomNum == roomNum; });
	
	if (findServerIter == roomInfoList.end())
	{
		PRINTF("Failed Search Room Number \n");
		LeaveCriticalSection(&serverLock);
		return false;
	}
	
	PRINTF("Complete Search Room Number \n");
	LeaveCriticalSection(&serverLock);
	return true;
	
	

}
