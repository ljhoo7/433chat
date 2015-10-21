#include "stdafx.h"
#include "ServerAgent.h"


ServerAgent::ServerAgent(TotalInfo* totalInfoData, int p_nThreadPoolSize, int p_nSocketPoolSize)
:m_nThreadPoolSize(p_nThreadPoolSize),
 m_nSocketPoolSize(p_nSocketPoolSize),
 isConnect(false),
 isUse(false),
 isVar(false)
{
	InitializeCriticalSectionAndSpinCount(&IOLock, 4000);

	m_pTotalInfoData = totalInfoData;

	m_pProactor     = new CProactor(p_nThreadPoolSize);
	m_pConnector    = new CConnector(m_pProactor);
	m_pDisconnector = new CDisconnector(m_pProactor);
	m_pReceiver		= new CReceiver(m_pProactor);
	m_pSender	    = new CSender(m_pProactor);

	m_pSock			= new SASocket(this);

	m_nPosition = 0;
	m_nRemainBytes = HEADER_SIZE;

	m_pSock->Init();
	m_pSock->InitAct(m_pProactor,
					 m_pDisconnector, 
					 m_pConnector, 
					 m_pSender,
					 m_pReceiver);

	static_cast<SASocket*>(m_pSock)->Bind(true);
	
	m_pProactor->Register((HANDLE)m_pSock->socket_);

	

	

	PRINTF("SASocket start....\n");
}


ServerAgent::~ServerAgent()
{
	DeleteCriticalSection(&IOLock);
}

void ServerAgent::SaveAgentNumber(unsigned short agentNum)
{
	EnterCriticalSection(&IOLock);

	m_pTotalInfoData->agentNum = agentNum;

	PRINTF("Agent Num : %d \n", agentNum);

	LeaveCriticalSection(&IOLock);
}

void ServerAgent::SaveDeltaRoomInfo(unsigned short roomNum, bool isState)
{
	EnterCriticalSection(&IOLock);

	if (isState == 1)
	{
		/// Created
		m_pTotalInfoData->roomCnt++;

		RoomInfo room;
		room.roomNum = roomNum;
		m_pTotalInfoData->roomInfoList.push_back(room);

	}
	else
	{
		/// Destroyed
		if (m_pTotalInfoData->roomCnt == 0)
		{
			PRINTF("ERROR : already room count is zero");
		}
		else
		{
			m_pTotalInfoData->roomCnt--;
			m_pTotalInfoData->roomInfoList.remove_if([&](RoomInfo& room){
				return room.roomNum == roomNum;
			});
		}
	}

	LeaveCriticalSection(&IOLock);
}

void ServerAgent::SaveDeltaUserInfo(int clientSocket, unsigned short roomNum, char* nickName, char isConnected)
{
	EnterCriticalSection(&IOLock);

	if (isConnected == 0)
	{
		/// Disconnected
		if (m_pTotalInfoData->userCnt == 0)
		{
			PRINTF("ERROR : already user count is zero");
		}
		else
		{
			m_pTotalInfoData->userCnt--;
			m_pTotalInfoData->userInfoList.remove_if([&](UserInfo& user){
				return (user.roomNum == roomNum) && 
					   (user.userSocket == clientSocket) && 
					   (strcmp(user.userName,nickName) == 0);
			});
		}

	}
	else if (isConnected == 1)
	{
		// Connected
		m_pTotalInfoData->userCnt++;

		UserInfo user;
		user.userSocket = clientSocket;
		user.roomNum = roomNum;
		strcpy_s(user.userName, NICK_SIZE, nickName);

		m_pTotalInfoData->userInfoList.push_back(user);

	}
	else if (isConnected == 2)
	{
		// Join
		auto finder = std::find_if(m_pTotalInfoData->userInfoList.begin(), 
					  m_pTotalInfoData->userInfoList.end(),
					  [&](UserInfo& user) {
					  return user.userSocket == clientSocket;
		});
		
		finder->roomNum = roomNum;
		memcpy(finder->userName, nickName, NICK_SIZE);

	}
	else if (isConnected == 3)
	{
		// Leave
		auto finder = std::find_if(m_pTotalInfoData->userInfoList.begin(),
			m_pTotalInfoData->userInfoList.end(),
			[&](UserInfo& user) {
			return user.userSocket == clientSocket;
		});

		finder->roomNum = roomNum;
		memcpy(finder->userName, nickName, NICK_SIZE);

	}
	else
	{
		PRINTF("ERROR : NOT VALID");
	}

	LeaveCriticalSection(&IOLock);
}

void ServerAgent::SaveDeltaInterSeverInfo(unsigned short serverNum, bool isConnected)
{
	EnterCriticalSection(&IOLock);

	if (isConnected == 0)
	{
		/// Disconnected
		if (m_pTotalInfoData->serverCnt == 0)
		{
			PRINTF("ERROR : already server count is zero");
		}
		else
		{
			m_pTotalInfoData->serverCnt--;
			m_pTotalInfoData->serverNumList.remove(serverNum);
		}
	}
	else if (isConnected == 1)
	{
		// Connected
		m_pTotalInfoData->serverCnt++;
		m_pTotalInfoData->serverNumList.push_back(serverNum);

	}
	

	LeaveCriticalSection(&IOLock);
}
void ServerAgent::SaveTotalRoomInfo(unsigned short roomCnt, RoomInfo* roomInfoList)
{
	EnterCriticalSection(&IOLock);

	if (!m_pTotalInfoData->roomInfoList.empty())
	{
		m_pTotalInfoData->roomInfoList.clear();
	}

	m_pTotalInfoData->roomCnt = roomCnt;

	for (int i = 0; i < roomCnt; ++i)
	{

		m_pTotalInfoData->roomInfoList.push_back(roomInfoList[i]);

	}

	LeaveCriticalSection(&IOLock);

}

void ServerAgent::SaveTotalUserInfo(unsigned short userCnt, UserInfo* userInfoList)
{
	EnterCriticalSection(&IOLock);

	if (!m_pTotalInfoData->userInfoList.empty())
	{
		m_pTotalInfoData->userInfoList.clear();
	}

	m_pTotalInfoData->userCnt = userCnt;

	for (int i = 0; i < userCnt; ++i)
	{

		m_pTotalInfoData->userInfoList.push_back(userInfoList[i]);

	}

	LeaveCriticalSection(&IOLock);
}

void ServerAgent::SaveTotalInterServerInfo(unsigned short serverCnt, unsigned short* serverNumList)
{
	EnterCriticalSection(&IOLock);
	
	if (!m_pTotalInfoData->serverNumList.empty())
	{
		m_pTotalInfoData->serverNumList.clear();
	}

	m_pTotalInfoData->serverCnt = serverCnt;
	
	for (int i = 0; i < serverCnt; ++i)
	{

		m_pTotalInfoData->serverNumList.push_back(serverNumList[i]);

	}

	LeaveCriticalSection(&IOLock);
}

void ServerAgent::PacketHandling(char* buf)
{
	sag_pkt_type eType = (sag_pkt_type)((unsigned short)(*buf));

	sag_tell_agent_number			 tellAgentNumPkt;
	sag_room_info_changed			 roomInfoChangedPkt;
	sag_user_info_changed			 userInfoChangedPkt;
	sag_interserver_connected		 interServerConnectedPkt;
	sag_total_room_info*			 totalRoomInfoPkt;
	sag_total_user_info*			 totalUserInfoPkt;
	sag_total_interserver_info*		 totalInterServerInfoPkt;

	switch (eType)
	{
	case sag_pkt_type::pt_room_info_changed:
		roomInfoChangedPkt = *((sag_room_info_changed*)(buf));
		
		SaveDeltaRoomInfo(roomInfoChangedPkt.roomNum, roomInfoChangedPkt.isState);

		break;
	case sag_pkt_type::pt_user_info_changed:
		userInfoChangedPkt = *((sag_user_info_changed*)(buf));
		
		SaveDeltaUserInfo(userInfoChangedPkt.clientSocket, 
			userInfoChangedPkt.roomNum,
			userInfoChangedPkt.userName,
			userInfoChangedPkt.isConnected);


		break;
	case sag_pkt_type::pt_interserver_connected:
		interServerConnectedPkt = *((sag_interserver_connected*)(buf));
		
		SaveDeltaInterSeverInfo(interServerConnectedPkt.serverNum, interServerConnectedPkt.isConnected);

		break;
	case sag_pkt_type::pt_total_room_info:
		PRINTF("sag_pkt_type::pt_total_room_info \n");

		totalRoomInfoPkt = (sag_total_room_info*)(buf);
		SaveTotalRoomInfo(totalRoomInfoPkt->roomCnt, totalRoomInfoPkt->roomInfoList);

		break; 
	case sag_pkt_type::pt_total_user_info:
		PRINTF("sag_pkt_type::pt_total_user_info \n");

		totalUserInfoPkt = (sag_total_user_info*)(buf);
		SaveTotalUserInfo(totalUserInfoPkt->userCnt, totalUserInfoPkt->userInfoList);
		break;

	case sag_pkt_type::pt_total_interserver_info:
		PRINTF("sag_pkt_type::pt_total_interserver_info \n");

		totalInterServerInfoPkt = (sag_total_interserver_info*)(buf);
		SaveTotalInterServerInfo(totalInterServerInfoPkt->serverCnt, totalInterServerInfoPkt->serverNumList);

		break;
	case sag_pkt_type::pt_tell_agent_number:
		tellAgentNumPkt = *((sag_tell_agent_number*)(buf));
		SaveAgentNumber(tellAgentNumPkt.agentNum);
		break;
	}
}


void ServerAgent::RecvProcess(bool isError, CAct* act, DWORD bytes_transferred)
{
	if (0 == bytes_transferred)
	{
		PRINTF("Disconnect in Receiver ProcEvent() \n");

		m_pSock->Disconnect();

		return;
	}
	
	
	m_nPosition += bytes_transferred;
	m_nRemainBytes -= bytes_transferred;
	
	//PRINTF("Server to Agent Packet Received %d bytes! \n",m_nRemainBytes);

	char *buf = m_pSock->recvBuf_;

	if (m_nPosition < HEADER_SIZE)
	{
		m_pSock->Recv(buf + m_nPosition, m_nRemainBytes);
	}
	else
	{
		if (m_nPosition == HEADER_SIZE)
		{
			sag_pkt_type eType = (sag_pkt_type)((unsigned short)(*buf));

			switch (eType)
			{
			case sag_pkt_type::pt_room_info_changed:
				PRINTF("pt_room_info_changed\n");
				m_nRemainBytes = sizeof(sag_room_info_changed)-HEADER_SIZE;

				break;
			case sag_pkt_type::pt_user_info_changed:
				PRINTF("pt_user_info_changed\n");
				m_nRemainBytes = sizeof(sag_user_info_changed)-HEADER_SIZE;

				break;
			case sag_pkt_type::pt_interserver_connected:
				PRINTF("pt_interserver_connected\n");
				m_nRemainBytes = sizeof(sag_interserver_connected)-HEADER_SIZE;

				break;
			case sag_pkt_type::pt_total_room_info:
			case sag_pkt_type::pt_total_user_info:
			case sag_pkt_type::pt_total_interserver_info:
				isVar = true;
				m_nRemainBytes = sizeof(short);
				break;
			case sag_pkt_type::pt_tell_agent_number:
				PRINTF("pt_tell_agent_number\n");
				m_nRemainBytes = sizeof(sag_tell_agent_number)-HEADER_SIZE;

				break;
			}
		}
		if (m_nRemainBytes <= 0)
		{
			if (isVar)
			{
				unsigned short cnt;
				memcpy(&cnt, buf + 2, sizeof(short));
					
				sag_pkt_type eType = (sag_pkt_type)((unsigned short)(*buf));

				switch (eType)
				{
					case sag_pkt_type::pt_total_room_info:
						m_nRemainBytes = cnt*sizeof(RoomInfo);
						PRINTF("m_nRemainBytes %d /sag_pkt_type::pt_total_room_info \n", m_nRemainBytes);
						break;
					case sag_pkt_type::pt_total_user_info:
						m_nRemainBytes = cnt*sizeof(UserInfo);
						PRINTF("m_nRemainBytes %d /sag_pkt_type::pt_total_user_info \n", m_nRemainBytes);

						break;
					case sag_pkt_type::pt_total_interserver_info:
						/// 서버에서 잘못 보내고있음.
						//m_nRemainBytes = cnt*sizeof(unsigned short);
						m_nRemainBytes = 0*sizeof(unsigned short);
						PRINTF("m_nRemainBytes %d /sag_pkt_type::pt_total_interserver_info \n", m_nRemainBytes);

						break;
				}
				isVar = false;

				if (m_nRemainBytes == 0)
				{
					m_nPosition = 0;
					m_nRemainBytes = HEADER_SIZE;
					PacketHandling(buf);
				}
			}
			else
			{
				m_nPosition = 0;
				m_nRemainBytes = HEADER_SIZE;
				
				PacketHandling(buf);


				/*CPacket* msg = packetPoolManager->Alloc();
				if (isConnect) msg->type = 0;
				else msg->type = 1;
				msg->owner = NULL;
				msg->msg = poolManager->Alloc()->buf;
				memcpy(msg->msg, buf, BUFSIZE);
				logicHandle.EnqueueOper(msg, true);*/
			}
		}
		m_pSock->Recv(buf + m_nPosition, m_nRemainBytes);

	}

}

void ServerAgent::SendProcess(bool isError, CAct* act, DWORD bytes_transferred)
{


}

void ServerAgent::DisconnProcess(bool isError, CAct* act, DWORD bytes_transferred)
{
	/*if (!isError){
		isUse = false;

		std::list<CPlayer*>::iterator iter;
		for (iter = players.begin(); iter != players.end();)
		{
			if (!(*iter)->isMine)
			{
				if ((*iter)->roomNum != -1)
				{
					roomManager.LeaveRoom((*iter), (*iter)->roomNum);
				}
				PRINTF("delete other server's user : %d\n", (*iter)->socket_);
				iter = players.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		PRINTF("closed the other server.\n");

		if (heartThread.joinable()) heartThread.join();
		if (!isConnect) this->socket->Reuse();
		else{
			static_cast<InterConnectSocket *>(socket)->Bind(false);
			proactor_->Register((HANDLE)socket->socket_);
		}
	}
	else{
		PRINTF("interServer disconnect error\n");
	}*/
}

void ServerAgent::ConnProcess(bool isError, CAct* act, DWORD bytes_transferred)
{
	if (!isError){
		this->isConnect = true;

		isUse = true;
		m_pSock->Recv(m_pSock->recvBuf_, HEADER_SIZE);
	}
	else{
		PRINTF("interServer connect error\n");
	}
}

void ServerAgent::Connect(DWORD ip, WORD port)
{
	SASocket* sock = static_cast<SASocket*>(m_pSock);
	sock->Connect(ip, port);
}