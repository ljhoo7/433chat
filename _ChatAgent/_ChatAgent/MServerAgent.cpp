#include "stdafx.h"
#include "MServerAgent.h"


#include "stdafx.h"
#include "ServerAgent.h"


MServerAgent::MServerAgent(TotalInfo* totalInfoData, int p_nThreadPoolSize, int p_nSocketPoolSize)
:m_nThreadPoolSize(p_nThreadPoolSize),
 m_nSocketPoolSize(p_nSocketPoolSize),
 isServerAgentConnected(false),
 isUse(false),
 isVar(false)
{
	InitializeCriticalSectionAndSpinCount(&IOLock, 4000);

	m_pServerAgentSocket = NULL;
	m_pTotalInfoData = totalInfoData;

	m_pProactor     = new CProactor(p_nThreadPoolSize);
	m_pConnector    = new CConnector(m_pProactor);
	m_pDisconnector = new CDisconnector(m_pProactor);
	m_pReceiver		= new CReceiver(m_pProactor);
	m_pSender		= new CSender(m_pProactor);

	m_pSock			= new MSASocket(this);

	m_nPosition = 0;
	m_nRemainBytes = HEADER_SIZE;

	m_pSock->Init();
	m_pSock->InitAct(m_pProactor,
		m_pDisconnector,
		m_pConnector,
		m_pSender,
		m_pReceiver);

	static_cast<MSASocket*>(m_pSock)->Bind(true);

	m_pProactor->Register((HANDLE)m_pSock->socket_);

	//isConnect = true;



	PRINTF("MSASocket start....\n");
}


MServerAgent::~MServerAgent()
{
	DeleteCriticalSection(&IOLock);
}

void MServerAgent::PacketHandling(char* buf)
{
	msag_pkt_type eType = (msag_pkt_type)((unsigned short)(*buf));

	// after received AGENT -> SERVER
	ags_user_out					 userOutPkt;
	ags_room_destroy				 roomDestroyPkt;
	ags_interserver_connect			 interServerConnectedPkt;
	ags_interserver_disconnect		 interServerDisConnectedPkt;

	// after received AGENT -> MONITORING SERVER
	msag_request_total_info			 totalSendInfoPkt;

	switch (eType)
	{
	case msag_pkt_type::pkt_user_out:
		userOutPkt = *((ags_user_out*)(buf));
		userOutPkt.type = sag_pkt_type::pt_user_out;

		if (m_pServerAgentSocket != nullptr)
			m_pServerAgentSocket->Send((char*)&userOutPkt, sizeof(ags_user_out));

		break;
	case msag_pkt_type::pkt_room_destroy:
		roomDestroyPkt = *((ags_room_destroy*)(buf));
		roomDestroyPkt.type = sag_pkt_type::pt_room_destroy;

		if (m_pServerAgentSocket != nullptr)
			m_pServerAgentSocket->Send((char*)&roomDestroyPkt, sizeof(ags_room_destroy));

		break;
	case msag_pkt_type::pkt_interserver_connect:
		interServerConnectedPkt = *(ags_interserver_connect*)(buf);
		interServerConnectedPkt.type = sag_pkt_type::pt_interserver_connect;

		if (m_pServerAgentSocket != nullptr)
			m_pServerAgentSocket->Send((char*)&interServerConnectedPkt, sizeof(pkt_interserver_connect));

		break;
	case msag_pkt_type::pkt_interserver_disconnect:
		interServerDisConnectedPkt = *(ags_interserver_disconnect*)(buf);
		interServerDisConnectedPkt.type = sag_pkt_type::pt_interserver_disconnect;

		if (m_pServerAgentSocket != nullptr)
			m_pServerAgentSocket->Send((char*)&interServerDisConnectedPkt, sizeof(pkt_interserver_disconnect));

		break;
	case msag_pkt_type::pkt_request_total_info:
		SendTotalData();
		/// MS가 TOTAL을 요청할때
		break;

	}
}
void MServerAgent::SendTotalData()
{
	SendRoomInfo();
	SendUserInfo();
	SendInterServerInfo();
}
void MServerAgent::SendUserInfo()
{
	agms_total_user_info pkt;
	pkt.type = msag_pkt_type::pkt_total_user_info;
	
	EnterCriticalSection(&IOLock);
	
	pkt.userCnt = m_pTotalInfoData->userCnt;

	int size = 0;
	size = sizeof(pkt.type) + sizeof(pkt.userCnt);

	if (!m_pTotalInfoData->userInfoList.empty())
	{
		int i = 0;
		for (auto& user : m_pTotalInfoData->userInfoList)
		{
			pkt.userInfoList[i] = user;
			++i;

			size += sizeof(UserInfo);
		}
	}
	LeaveCriticalSection(&IOLock);
	
	m_pSock->Send((char*)&pkt, size);

}

void MServerAgent::SendRoomInfo()
{
	agms_total_room_info pkt;
	pkt.type = msag_pkt_type::pkt_total_room_info;

	EnterCriticalSection(&IOLock);

	pkt.roomCnt = m_pTotalInfoData->roomCnt;

	int size = 0;
	size = sizeof(pkt.type) + sizeof(pkt.roomCnt);

	if (!m_pTotalInfoData->roomInfoList.empty())
	{
		int i = 0;
		for (auto& room : m_pTotalInfoData->roomInfoList)
		{
			pkt.roomInfoList[i] = room;
			++i;

			size += sizeof(RoomInfo);
		}
	}
	LeaveCriticalSection(&IOLock);

	m_pSock->Send((char*)&pkt, size);

}

void MServerAgent::SendInterServerInfo()
{
	agms_total_interserver_info pkt;
	pkt.type = msag_pkt_type::pkt_total_interserver_info;

	EnterCriticalSection(&IOLock);

	pkt.serverCnt = m_pTotalInfoData->serverCnt;

	int size = 0;
	size = sizeof(pkt.type) + sizeof(pkt.serverCnt);

	if (!m_pTotalInfoData->serverNumList.empty())
	{
		int i = 0;
		for (auto& serverNum : m_pTotalInfoData->serverNumList)
		{
			pkt.serverNumList[i] = serverNum;
			++i;

			size += sizeof(unsigned short);
		}
	}
	LeaveCriticalSection(&IOLock);

	m_pSock->Send((char*)&pkt, size);

}

void MServerAgent::RecvProcess(bool isError, CAct* act, DWORD bytes_transferred)
{
	if (0 == bytes_transferred)
	{
		PRINTF("Disconnect in Receiver ProcEvent() \n");

		m_pSock->Disconnect();

		return;
	}

	m_nPosition += bytes_transferred;
	m_nRemainBytes -= bytes_transferred;

	//PRINTF("MServer to Agent Packet Received %d bytes! \n", m_nRemainBytes);

	char *buf = m_pSock->recvBuf_;

	if (m_nPosition < HEADER_SIZE)
	{
		m_pSock->Recv(buf + m_nPosition, m_nRemainBytes);
	}
	else
	{
		if (m_nPosition == HEADER_SIZE)
		{
			msag_pkt_type eType = (msag_pkt_type)((unsigned short)(*buf));

			switch (eType)
			{
			case msag_pkt_type::pkt_user_out:
				PRINTF("received user out packet\n");
				m_nRemainBytes = sizeof(msag_user_out)-HEADER_SIZE;

				break;
			case msag_pkt_type::pkt_room_destroy:
				PRINTF("received room destroy packet\n");
				m_nRemainBytes = sizeof(msag_room_destroy)-HEADER_SIZE;

				break;
			case msag_pkt_type::pkt_interserver_connect:
				PRINTF("received interserver connect packet\n");
				m_nRemainBytes = sizeof(msag_interserver_connect)-HEADER_SIZE;

				break;
			case msag_pkt_type::pkt_interserver_disconnect:
				PRINTF("received interserver disconnect packet\n");
				m_nRemainBytes = sizeof(msag_interserver_disconnect)-HEADER_SIZE;

				break;
			case msag_pkt_type::pkt_request_total_info:
				PRINTF("received request total info\n");
				m_nRemainBytes = sizeof(msag_request_total_info)-HEADER_SIZE;

				break;
			}
		}
		if (m_nRemainBytes <= 0)
		{
			m_nPosition = 0;
			m_nRemainBytes = HEADER_SIZE;

			PacketHandling(buf);

		}
		m_pSock->Recv(buf + m_nPosition, m_nRemainBytes);
	}
}

void MServerAgent::SendProcess(bool isError, CAct* act, DWORD bytes_transferred)
{


}

void MServerAgent::DisconnProcess(bool isError, CAct* act, DWORD bytes_transferred)
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

void MServerAgent::ConnProcess(bool isError, CAct* act, DWORD bytes_transferred)
{
	if (!isError)
	{
		isUse = true;
		agms_tell_agent_number pkt;
		pkt.type = msag_pkt_type::pkt_tell_agent_number;
		pkt.agentNum = m_pTotalInfoData->agentNum;
		
		m_pSock->Send((char*)&pkt, sizeof(agms_tell_agent_number));

		m_pSock->Recv(m_pSock->recvBuf_, HEADER_SIZE);
	}
	else{
		PRINTF("Monitoring Server connect error\n");
	}
}

void MServerAgent::Connect(DWORD ip, WORD port){

	if (isServerAgentConnected)
	{
		
		MSASocket* sock = static_cast<MSASocket*>(m_pSock);
		sock->Connect(ip, port);
	}
	else
	{
		PRINTF("Server to Agent Not Connected !\n");

	}
}
void MServerAgent::IsConnected(ServerAgent* pServerAgent)
{
	if (pServerAgent->isConnect)
	{
		isServerAgentConnected = true;
		m_pServerAgentSocket   = pServerAgent->m_pSock;
	}
	else
		isServerAgentConnected = false;

}