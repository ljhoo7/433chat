#include "stdafx.h"

SASocket::SASocket()
:mServerNum(0),
 mPosition(0),
 mRemainBytes(HEADER_SIZE),
 mIsVar(false)
{
}

SASocket::~SASocket()
{

}

void SASocket::PacketHandling(char* buf)
{
	sag_pkt_type eType = (sag_pkt_type)((unsigned short)(*buf));

	sag_room_info_changed			 roomInfoChangedPkt;
	sag_user_info_changed			 userInfoChangedPkt;
	sag_server_info_changed			 serverInfoChangedPkt;
	sag_total_room_info*			 totalRoomInfoPkt;
	sag_total_user_info*			 totalUserInfoPkt;
	sag_total_interserver_info*		 totalInterServerInfoPkt;

	switch (eType)
	{
	case sag_pkt_type::pt_room_info_changed:
		PRINTF("sag_pkt_type::pt_room_info_changed \n");
		roomInfoChangedPkt = *((sag_room_info_changed*)(buf));

		AgentApp::Instance()->SaveDeltaRoomInfo(roomInfoChangedPkt.roomNum, roomInfoChangedPkt.isState);

		break;
	case sag_pkt_type::pt_user_info_changed:
		PRINTF("sag_pkt_type::pt_user_info_changed \n");

		userInfoChangedPkt = *((sag_user_info_changed*)(buf));

		AgentApp::Instance()->SaveDeltaUserInfo(mServerNum,
			userInfoChangedPkt.clientSocket,
			userInfoChangedPkt.roomNum,
			userInfoChangedPkt.userName,
			userInfoChangedPkt.isConnected);


		break;
	case sag_pkt_type::pt_server_info_changed:
		PRINTF("sag_pkt_type::pt_user_info_changed \n");

		serverInfoChangedPkt = *((sag_server_info_changed*)(buf));

		AgentApp::Instance()->SaveDeltaInterSeverInfo(serverInfoChangedPkt.serverNum, serverInfoChangedPkt.isConnected);

		break;
	case sag_pkt_type::pt_total_room_info:
		PRINTF("sag_pkt_type::pt_total_room_info \n");

		totalRoomInfoPkt = (sag_total_room_info*)(buf);
		AgentApp::Instance()->SaveTotalRoomInfo(totalRoomInfoPkt->roomCnt, totalRoomInfoPkt->roomInfoList);

		break;
	case sag_pkt_type::pt_total_user_info:
		PRINTF("sag_pkt_type::pt_total_user_info \n");

		totalUserInfoPkt = (sag_total_user_info*)(buf);
		AgentApp::Instance()->SaveTotalServerUserInfo(mServerNum,totalUserInfoPkt->userCnt, totalUserInfoPkt->userInfoList);
		break;

	case sag_pkt_type::pt_total_interserver_info:
		PRINTF("sag_pkt_type::pt_total_interserver_info \n");

		totalInterServerInfoPkt = (sag_total_interserver_info*)(buf);
		AgentApp::Instance()->SaveTotalInterServerInfo(totalInterServerInfoPkt->serverCnt, totalInterServerInfoPkt->serverNumList);

		break;

	}

}
void SASocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (0 == bytes_transferred)
	{
		PRINTF("Disconnect in Receiver ProcEvent() \n");

		Disconnect();

		return;
	}

	mPosition += bytes_transferred;
	mRemainBytes -= bytes_transferred;

	//PRINTF("Server to Agent Packet Received %d bytes! \n",m_nRemainBytes);

	char *buf = this->recvBuf_;

	if (mPosition < HEADER_SIZE)
	{
		this->Recv(buf + mPosition, mRemainBytes);
	}
	else
	{
		if (mPosition == HEADER_SIZE)
		{
			sag_pkt_type eType = (sag_pkt_type)((unsigned short)(*buf));

			switch (eType)
			{
			case sag_pkt_type::pt_room_info_changed:
				PRINTF("pt_room_info_changed\n");
				mRemainBytes = sizeof(sag_room_info_changed)-HEADER_SIZE;

				break;
			case sag_pkt_type::pt_user_info_changed:
				PRINTF("pt_user_info_changed\n");
				mRemainBytes = sizeof(sag_user_info_changed)-HEADER_SIZE;

				break;
			case sag_pkt_type::pt_server_info_changed:
				PRINTF("pt_server_info_changed\n");
				mRemainBytes = sizeof(sag_server_info_changed)-HEADER_SIZE;

				break;
			case sag_pkt_type::pt_total_room_info:
			case sag_pkt_type::pt_total_user_info:
			case sag_pkt_type::pt_total_interserver_info:
				mIsVar = true;
				mRemainBytes = sizeof(short);
				break;
			
			case sag_pkt_type::pt_health_ack:
				PRINTF("pt_pt_health_ack\n");
				mRemainBytes = sizeof(sag_health_ack)-HEADER_SIZE;

				break;
			}

		}
		if (mRemainBytes <= 0)
		{
			if (mIsVar)
			{
				unsigned short cnt;
				memcpy(&cnt, buf + 2, sizeof(short));

				sag_pkt_type eType = (sag_pkt_type)((unsigned short)(*buf));

				switch (eType)
				{
				case sag_pkt_type::pt_total_room_info:
					
					//PRINTF("m_nRemainBytes %d /sag_pkt_type::pt_total_room_info \n", mRemainBytes);
					mRemainBytes = cnt*sizeof(RoomInfo);
					break;
				case sag_pkt_type::pt_total_user_info:
					
					//PRINTF("m_nRemainBytes %d /sag_pkt_type::pt_total_user_info \n", mRemainBytes);
					mRemainBytes = cnt*sizeof(UserInfo);

					break;
				case sag_pkt_type::pt_total_interserver_info:
					
					//PRINTF("m_nRemainBytes %d /sag_pkt_type::pt_total_interserver_info \n", mRemainBytes);
					mRemainBytes = cnt * sizeof(unsigned short);

					break;
				}
				mIsVar = false;

				if (mRemainBytes == 0)
				{
					mPosition = 0;
					mRemainBytes = HEADER_SIZE;
					PacketHandling(buf);
				}
			}
			else
			{
				mPosition = 0;
				mRemainBytes = HEADER_SIZE;

				PacketHandling(buf);
			}
		}
		this->Recv(buf + mPosition, mRemainBytes);

	}
}
void SASocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	
	if (!isError)
	{
		
		PRINTF("Connect Server Success, %d\n", this->socket_);
		

		memcpy(&mServerNum, this->acceptBuf_, sizeof(mServerNum));
		PRINTF("Server Number : %d \n", mServerNum);

		AgentApp::Instance()->AddServer(this);
		Recv(this->recvBuf_, HEADER_SIZE);
	}
	else
	{
		/* error handling */
		PRINTF("CPlayer AcceptProcess : Error : %d\n", WSAGetLastError());
	}

}

void SASocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	
}

void SASocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	if (!isError)
	{
		
	}
	else
	{

	}
	
}

void SASocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred)
{
	
}

