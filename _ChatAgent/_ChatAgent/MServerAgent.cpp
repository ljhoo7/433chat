#include "stdafx.h"
#include "MServerAgent.h"

MServerAgent::MServerAgent(int p_nThreadPoolSize, int p_nSocketPoolSize)
:WinSockBase(),
 m_nThreadPoolSize(p_nThreadPoolSize),
 m_nSocketPoolSize(p_nSocketPoolSize),
 m_nPosition(0),
 m_nRemainBytes(HEADER_SIZE),
 isUse(false),
 isVar(false)
{
	InitializeCriticalSectionAndSpinCount(&IOLock, 4000);

	mProactor				= new Proactor(p_nThreadPoolSize);
	mConnector			    = new Connector(mProactor);
	mDisconnector			= new Disconnector(mProactor);
	mReceiver			    = new Receiver(mProactor);
	mSender					= new Sender(mProactor);

	m_pSock					= new MSASocket(static_cast<MServerAgent*>(this));

	m_pSock->Init();
	m_pSock->InitAct(mProactor,
		NULL,
		mDisconnector,
		mConnector,
		mSender,
		mReceiver);

	static_cast<MSASocket*>(m_pSock)->Bind(true);

	mProactor->Register((HANDLE)m_pSock->socket_);

	PRINTF("MSASocket start....\n");
}


MServerAgent::~MServerAgent()
{
	if (mProactor)		delete mProactor;
	if (mConnector)		delete mConnector;
	if (mDisconnector)	delete mDisconnector;
	if (mReceiver)		delete mReceiver;
	if (mSender)		delete mSender;
	if (m_pSock)		delete m_pSock;


	DeleteCriticalSection(&IOLock);
}

void MServerAgent::PacketHandling(char* buf)
{
	msag_pkt_type eType = (msag_pkt_type)((unsigned short)(*buf));

	// after received AGENT -> SERVER
	msag_user_out					 userOutPkt;
	msag_room_destroy				 roomDestroyPkt;
	msag_generate_server			 generateServerPkt;
	msag_kill_server				 killServerPkt;
	
	switch (eType)
	{
	case msag_pkt_type::pkt_user_out:
		userOutPkt = *((msag_user_out*)(buf));
		SendUserOut(userOutPkt.serverNum, userOutPkt.userSocket);
	
		break;
	case msag_pkt_type::pkt_room_destroy:
		roomDestroyPkt = *((msag_room_destroy*)(buf));
		SendRoomDestroy(roomDestroyPkt.roomNum);

		break;
	case msag_pkt_type::pkt_generate_server:
		generateServerPkt = *((msag_generate_server*)(buf));
		SendGenerateServer();

		break;
	case msag_pkt_type::pkt_kill_server:
		killServerPkt = *((msag_kill_server*)(buf));
		SendKillServer(killServerPkt.serverNum);

		break;
	case msag_pkt_type::pkt_total_request:
		PRINTF("Send Total Data\n");
		SendTotalData();
		
		break;

	}
}
void MServerAgent::SendUserOut(int serverNum, int userSocket)
{
	bool isSearch = AgentApp::Instance()->IsSearchUser(serverNum, userSocket);

	if (isSearch)
	{
		// Success, Send Server
		ags_user_out serverPkt;
		serverPkt.type = sag_pkt_type::pt_user_out;
		serverPkt.userSocket = userSocket;
		
		SASocket* pServerSocket = AgentApp::Instance()->FindServer(serverNum);
		
		PRINTF("Send to Server User Out Packet\n");
		pServerSocket->Send((char*)&serverPkt, sizeof(ags_user_out));


		agms_user_out_success mserverPkt;
		mserverPkt.type = msag_pkt_type::pkt_user_out_success;
		
		PRINTF("Send to Monitoring Server User Out Success Packet\n");
		m_pSock->Send((char*)&mserverPkt, sizeof(agms_user_out_success));
	}
	else
	{
		// Failed, Return Monitoring Server
		agms_user_out_fail pkt;
		pkt.type = msag_pkt_type::pkt_user_out_fail;
		pkt.failSignal = fail_signal::fs_no_exist;

		PRINTF("Send to Monitoring Server User Out Failed Packet\n");
		m_pSock->Send((char*)&pkt, sizeof(agms_user_out_fail));
	}
}


void MServerAgent::SendRoomDestroy(int roomNum)
{
	bool isSearch = AgentApp::Instance()->IsSearchRoom(roomNum);

	if (isSearch)
	{
		// Success, Send Server
		ags_room_destroy serverPkt;
		serverPkt.type = sag_pkt_type::pt_room_destroy;
		serverPkt.roomNum = roomNum;

		SASocket* pServerSocket = AgentApp::Instance()->GetServerSocketList().back();
		PRINTF("Send to Server Room Destroy Packet\n");
		pServerSocket->Send((char*)&serverPkt, sizeof(ags_room_destroy));

		agms_room_destroy_success mserverPkt;
		mserverPkt.type = msag_pkt_type::pkt_room_destroy_success;
		
		PRINTF("Send to Monitoring Server Room Destroy Success Packet\n");
		m_pSock->Send((char*)&mserverPkt, sizeof(agms_room_destroy_success));
	}
	else
	{
		// Failed, Return Monitoring Server
		agms_room_destroy_fail pkt;
		
		pkt.type = msag_pkt_type::pkt_room_destroy_fail;
		pkt.failSignal = fail_signal::fs_no_exist;

		PRINTF("Send to Monitoring Server Room Destroy Failed Packet\n");
		m_pSock->Send((char*)&pkt, sizeof(agms_room_destroy_fail));
	}

}

void MServerAgent::SendGenerateServer()
{
	unsigned int serverCount = AgentApp::Instance()->GetServerCount();

	if (0 == serverCount)
	{
		bool isGenerate = AgentApp::Instance()->GenerateServerProcess();

		if (isGenerate)
		{
			PRINTF("Real Generate Server Success\n");

		}
		else
		{
			PRINTF("Real Generate Server Failed\n");
		}
	}
	else
	{
		PRINTF("Already Generate All Server \n");

		agms_generate_server_fail pkt;
		
		pkt.type = msag_pkt_type::pkt_generate_server_fail;
		pkt.failSignal = fail_signal::fs_alreadyexist;

		PRINTF("Send to Monitoring Server Generate Server Failed Packet\n");
		m_pSock->Send((char*)&pkt, sizeof(agms_generate_server_fail));
	}
}

void MServerAgent::SendKillServer(int serverNum)
{
	SASocket* pServerSocket = AgentApp::Instance()->FindServer(serverNum);

	if (pServerSocket)
	{
		// Success, Send Server
		PRINTF("Success Find Server \n");
		
		ags_kill_server serverPkt;
		serverPkt.type = sag_pkt_type::pt_kill_server;
		PRINTF("Send to Server Kill Server Packet\n");
		pServerSocket->Send((char*)&serverPkt, sizeof(ags_kill_server));
		
		
	}
	else
	{
		// Failed, Send Monitoring Server
		PRINTF("Failed Find Server \n");

		agms_kill_server_fail pkt;
		pkt.type = msag_pkt_type::pkt_kill_server_fail;
		pkt.failSignal = fail_signal::fs_no_exist;

		
		PRINTF("Send to Monitoring Server Kill Server Failed Packet\n");
		m_pSock->Send((char*)&pkt, sizeof(agms_kill_server_fail));

	}

}

void MServerAgent::SendTotalData()
{
	SendServerCount();
	SendRoomInfo();
	SendServerInfo();
}
void MServerAgent::SendServerCount()
{
	EnterCriticalSection(&IOLock);

	agms_total_server_count pkt;
	pkt.type = msag_pkt_type::pkt_total_server_count;
	pkt.serverCnt = AgentApp::Instance()->GetTotalInfoData()->serverUserInfoList.size();

	PRINTF("Send Server Count Packet\n");

	m_pSock->Send((char*)&pkt, sizeof(agms_total_server_count));

	LeaveCriticalSection(&IOLock);

}
void MServerAgent::SendServerInfo()
{
	EnterCriticalSection(&IOLock);
	
	std::list<ServerInfo>& serverInfoList = AgentApp::Instance()->GetTotalInfoData()->serverUserInfoList;

	if (!serverInfoList.empty())
	{
		for (auto& serverInfo : serverInfoList)
		{
			agms_total_user_info pkt;
			pkt.type = msag_pkt_type::pkt_total_user_info;
			pkt.serverNum = serverInfo.serverNum;
			pkt.userCnt = serverInfo.userCount;
			
			int i = 0;
			int size = sizeof(pkt.type) + sizeof(pkt.serverNum) + sizeof(pkt.userCnt);
			for (auto& user : serverInfo.userInfoList)
			{
				pkt.userInfoList[i] = user;
				++i;

				size += sizeof(UserInfo);
			}
			
			PRINTF("Send Server Information Packet\n");

			m_pSock->Send((char*)&pkt, size);

		}
		
	}

	LeaveCriticalSection(&IOLock);
}

void MServerAgent::SendRoomInfo()
{
	agms_total_room_info pkt;
	pkt.type = msag_pkt_type::pkt_total_room_info;

	EnterCriticalSection(&IOLock);

	pkt.roomCnt = AgentApp::Instance()->GetTotalInfoData()->roomCnt;

	int size = 0;
	size = sizeof(pkt.type) + sizeof(pkt.roomCnt);

	std::list<RoomInfo>& roomList = AgentApp::Instance()->GetTotalInfoData()->roomInfoList;

	if (!roomList.empty())
	{
		int i = 0;
		for (auto& room : roomList)
		{
			pkt.roomInfoList[i] = room;
			++i;

			size += sizeof(RoomInfo);
		}
	}
	LeaveCriticalSection(&IOLock);

	PRINTF("Send Room Information Packet\n");

	m_pSock->Send((char*)&pkt, size);

}

void MServerAgent::RecvProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (0 == bytes_transferred)
	{
		PRINTF("Disconnect in Receiver ProcEvent() \n");

		m_pSock->Disconnect();

		return;
	}

	m_nPosition += bytes_transferred;
	m_nRemainBytes -= bytes_transferred;

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
			case msag_pkt_type::pkt_kill_server:
				PRINTF("received kill server packet\n");
				m_nRemainBytes = sizeof(msag_kill_server)-HEADER_SIZE;

				break;
			case msag_pkt_type::pkt_total_request:
				PRINTF("received total request packet\n");
				m_nRemainBytes = sizeof(msag_request_total_info)-HEADER_SIZE;

				break;

			case msag_pkt_type::pkt_generate_server:
				PRINTF("received generate server packet\n");
				m_nRemainBytes = sizeof(msag_generate_server)-HEADER_SIZE;

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

void MServerAgent::SendProcess(bool isError, Act* act, DWORD bytes_transferred)
{

}

void MServerAgent::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (!isError)
	{
		agms_kill_server_success mserverPkt;
		mserverPkt.type = msag_pkt_type::pkt_kill_server_success;
		PRINTF("Send to Monitoring Server Kill Server Success Packet\n");
		m_pSock->Send((char*)&mserverPkt, sizeof(agms_kill_server_success));
	}
	else
	{

	}
	
}

void MServerAgent::ConnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (!isError)
	{
		// Wait Recv
		m_pSock->Recv(m_pSock->recvBuf_, HEADER_SIZE);
	}
	else{
		PRINTF("Monitoring Server connect error\n");
	}
}

void MServerAgent::Connect(DWORD ip, WORD port)
{
	MSASocket* sock = static_cast<MSASocket*>(m_pSock);
	sock->Connect(ip, port);
}
