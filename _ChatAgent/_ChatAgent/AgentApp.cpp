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
	int threadPoolSize = nCPUs * 2;

	mTotalInfoData			= new TotalInfo;
	mServerAgent		    = new ServerAgent(agentPort, threadPoolSize, 100);
	mMonitoringServerAgent  = new MServerAgent(threadPoolSize, 1);

	mServerAgent->Run();
}


void AgentApp::Destroy()
{
	/// 공유 데이터는 프레임워크 클래스에서 소멸
	if (mTotalInfoData)			delete mTotalInfoData;
	if (mServerAgent)			delete mServerAgent;
	if (mMonitoringServerAgent) delete mMonitoringServerAgent;

	if (!mServerSocketList.empty())
	{
		for (auto& socket : mServerSocketList)
		{
			if (socket)
			{
				delete socket;
				socket = nullptr;
			}
		}
		mServerSocketList.clear();
	}

	for (auto& waitThreads : mTimeWaitThreads)
	{
		if (waitThreads.joinable())
		{
			waitThreads.join();
		}
	}
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
		PRINTF("\n\n");

	}
	else if (input == "generate")
	{
		PRINTF("Generate Server Process...\n");
		GenerateServerProcess();

		PRINTF("\n");
	}
	else if (input == "quit")
	{
		mIsExit = true;
	}
}


bool AgentApp::GenerateServerProcess()
{
	bool isAllAlive = false;

	EnterCriticalSection(&serverLock);

	std::string fileName = "GenerateServerList.txt";
	std::ifstream fin(fileName);

	if (!fin)
	{
		std::cerr << fileName << " 파일이 열리지 않습니다 " << std::endl;

		return false;
	}
	
	std::string LABLE;
	std::string PROCESS_NAME;
	std::string SERVER_NUM;
	std::string AGENT_PORT;
	WORD serverCount = 0;

	fin >> LABLE >> serverCount;

	for (int i = 0; i < serverCount; ++i)
	{
		fin >> PROCESS_NAME >> SERVER_NUM >> AGENT_PORT;
		
		SASocket* pSocket = FindServer(atoi(SERVER_NUM.c_str()));
		if (pSocket)
		{
			isAllAlive = true;
			continue;
		}


		isAllAlive = false;
		break;
	}
	
	if (!isAllAlive)
	{

		std::string commandStr = PROCESS_NAME + ' ' + SERVER_NUM + ' ' + AGENT_PORT;


		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(STARTUPINFO));

		si.cb = sizeof(si);
		si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
		si.dwX = 100;
		si.dwY = 100;
		si.dwXSize = 800;
		si.dwYSize = 300;

		si.lpTitle = _T("Child Server");

		bool isGenerate;

		std::wstring commandLine = StringToWstring(commandStr);

		isGenerate = ::CreateProcess(NULL,
									 const_cast<wchar_t*>(commandLine.c_str()),
									 NULL, NULL,
									 TRUE,
									 CREATE_NEW_CONSOLE,
									 NULL, NULL,
									 &si,
									 &pi);

									 
		if (isGenerate == 0)
		{
			fin.close();

			PRINTF("ERROR : Creating Error\n");
			LeaveCriticalSection(&serverLock);
			return false;
		}

		CloseHandle(pi.hThread);

		/// Add ProcessInfo 
		AddProcessInfo(atoi(SERVER_NUM.c_str()), false, pi.hProcess);
		
	}
	else
	{
		PRINTF("ERROR : Already All Server Alive \n");
		fin.close();
		LeaveCriticalSection(&serverLock);
		return false;
	}

	mTimeWaitThreads.push_back(std::thread(&AgentApp::GenerateTimeWait, this, atoi(SERVER_NUM.c_str())));

	fin.close();

	LeaveCriticalSection(&serverLock);

	return true;
	
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

	for (auto& socket : mServerSocketList)
	{
		if (socket)
		{
			if (socket->mServerNum == serverNum)
			{
				LeaveCriticalSection(&serverLock);
				return socket;
			}
		}
	}
	LeaveCriticalSection(&serverLock);
	return NULL;
}


bool AgentApp::IsProcessConnected(int serverNum)
{
	EnterCriticalSection(&serverLock);

	for (const auto& processInfo : mProcessCheckList)
	{
		if (processInfo.serverNum == serverNum)
		{
			LeaveCriticalSection(&serverLock);
			return processInfo.isConnected;
		}

	}

	LeaveCriticalSection(&serverLock);
	return false;

}

void AgentApp::DeleteProcessInfo(int serverNum)
{
	EnterCriticalSection(&serverLock);

	auto findProcessIter = std::find_if(mProcessCheckList.begin(),
		mProcessCheckList.end(),
		[&](const ProcessInfo& processInfo){ return processInfo.serverNum == serverNum; });


	if (findProcessIter == mProcessCheckList.end())
	{
		PRINTF("Failed Search Process Info \n");
		LeaveCriticalSection(&serverLock);
		return;
	}

	PRINTF("Success Delete Process Info \n");
	mProcessCheckList.remove((*findProcessIter));

	LeaveCriticalSection(&serverLock);

	return;

}
const HANDLE& AgentApp::GetProcessInfoHandle(int serverNum)
{
	EnterCriticalSection(&serverLock);

	for (const auto& processInfo : mProcessCheckList)
	{
		if (processInfo.serverNum == serverNum)
		{
			LeaveCriticalSection(&serverLock);
			return processInfo.processHandle;

		}
	}

	LeaveCriticalSection(&serverLock);
}
void AgentApp::SetProcessConnected(int serverNum, bool connected)
{
	EnterCriticalSection(&serverLock);

	for (auto& processInfo : mProcessCheckList)
	{
		if (processInfo.serverNum == serverNum)
		{
			processInfo.isConnected = connected;

			LeaveCriticalSection(&serverLock);
			return;
		}
	}

	LeaveCriticalSection(&serverLock);
}

void AgentApp::AddProcessInfo(int serverNum, bool connected, const HANDLE& hProcess)
{
	EnterCriticalSection(&serverLock);
	
	mProcessCheckList.push_back(ProcessInfo(serverNum, connected, hProcess));
	
	LeaveCriticalSection(&serverLock);
}

void AgentApp::SaveDeltaRoomInfo(unsigned short roomNum, bool isState)
{
	EnterCriticalSection(&serverLock);

	if (TRANSLATE_STATE::ROOM_STATE::CREATED == isState)
	{
		/// Created
		mTotalInfoData->roomCnt++;

		RoomInfo room;
		room.roomNum = roomNum;
		mTotalInfoData->roomInfoList.push_back(room);

	}
	else if (TRANSLATE_STATE::ROOM_STATE::DESTROYED == isState)
	{
		/// Destroyed
		if (0 == mTotalInfoData->roomCnt)
		{
			PRINTF("ERROR : already room count is zero");
		}
		else
		{
			mTotalInfoData->roomCnt--;
			mTotalInfoData->roomInfoList.remove_if([&](const RoomInfo& room){
				return room.roomNum == roomNum;
			});
		}
	}
	else
	{
		PRINTF("ERROR : Not Valid State Number\n");
	}

	LeaveCriticalSection(&serverLock);
}

void AgentApp::SaveDeltaUserInfo(unsigned int serverNum, int clientSocket, unsigned short roomNum, char* nickName, char isConnected)
{
	EnterCriticalSection(&serverLock);

	std::list<ServerInfo>& serverInfoList = mTotalInfoData->serverUserInfoList;

	auto serverInfoIter = std::find_if(serverInfoList.begin(), 
		serverInfoList.end(), 
		[&](const ServerInfo& serverInfo){
		return serverInfo.serverNum == serverNum;
	});


	if (serverInfoIter == serverInfoList.end())
	{
		PRINTF("ERROR : Not Exist Server Number \n");
		
		LeaveCriticalSection(&serverLock);

		return;
	}
	




	if (TRANSLATE_STATE::USER_STATE::DISCONNECTED == isConnected)
	{
		if (0 == serverInfoIter->userCount)
		{
			PRINTF("ERROR : already user count is zero \n");
		}
		else
		{
			PRINTF("User Disconnect! \n ");
			serverInfoIter->userCount--;

		
			serverInfoIter->userInfoList.remove_if([&](const UserInfo& user){
				//PRINTF("Room Number : (%d, %d) / Socket Number : (%d, %d)\n", user.roomNum, roomNum, user.userSocket, clientSocket);
				return (user.userSocket == clientSocket);
			
			});
		}

	}
	else if (TRANSLATE_STATE::USER_STATE::CONNECTED == isConnected)
	{
		PRINTF("User Connect! \n ");
		serverInfoIter->userCount++;

		UserInfo user;
		user.userSocket = clientSocket;
		user.roomNum = roomNum;
		strcpy_s(user.userName, NICK_SIZE, nickName);

		serverInfoIter->userInfoList.push_back(user);

	}
	else if (TRANSLATE_STATE::USER_STATE::JOIN == isConnected)
	{
		PRINTF("User Join! \n ");
		auto userIter = std::find_if(serverInfoIter->userInfoList.begin(),
			serverInfoIter->userInfoList.end(),
			[&](const UserInfo& user) {
			return user.userSocket == clientSocket;
		});

		userIter->roomNum = roomNum;
		memcpy(userIter->userName, nickName, NICK_SIZE);

	}
	else if (TRANSLATE_STATE::USER_STATE::LEAVE == isConnected)
	{
		PRINTF("User Leave! \n ");

		auto userIter = std::find_if(serverInfoIter->userInfoList.begin(),
			serverInfoIter->userInfoList.end(),
			[&](const UserInfo& user) {
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

	bool isExist = false;
	for (auto& serverInfo : mTotalInfoData->serverUserInfoList)
	{
		if (serverInfo.serverNum == serverNum)
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


bool AgentApp::DeleteServerInfo(int serverNum)
{
	EnterCriticalSection(&serverLock);

	std::list<ServerInfo>& serverInfoList = mTotalInfoData->serverUserInfoList;

	auto findServerIter = std::find_if(serverInfoList.begin(),
		serverInfoList.end(),
		[&](const ServerInfo& serverInfo){ return serverInfo.serverNum == serverNum; });

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
									   [&](const ServerInfo& serverInfo){ return serverInfo.serverNum == serverNum; });
	
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
		[&](const RoomInfo& roomInfo){ return roomInfo.roomNum == roomNum; });
	
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

void AgentApp::GenerateTimeWait(int serverNum)
{
	PRINTF("Start Wait\n");

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	while (1)
	{
		std::chrono::system_clock::time_point end = std::chrono::system_clock::now();

		std::chrono::duration<double> sec = end - start;

		if (IsProcessConnected(serverNum))
		{
			agms_generate_server_success pkt;
			pkt.type = msag_pkt_type::pkt_generate_server_success;

			mMonitoringServerAgent->m_pSock->Send((char*)&pkt, sizeof(agms_generate_server_success));
			
			HANDLE hProcess = GetProcessInfoHandle(serverNum);
			CloseHandle(hProcess);

			PRINTF("Generate Real Success\n");
			break;
		}

		if (WAIT_TIME <= sec.count())
		{
			agms_generate_server_fail pkt;
			pkt.type = msag_pkt_type::pkt_generate_server_fail;
			pkt.failSignal = fail_signal::fs_overflow;

			mMonitoringServerAgent->m_pSock->Send((char*)&pkt, sizeof(agms_generate_server_fail));

			PRINTF("ERROR : Generate Failed \n");
			PRINTF("TerminateProcess !! \n");

			HANDLE hProcess = GetProcessInfoHandle(serverNum);

			TerminateProcess(hProcess, NULL);

			CloseHandle(hProcess);

			DeleteProcessInfo(serverNum);

			
			break;
		}

	}

	PRINTF("Exit Wait\n");
}