#pragma once

#include "stdafx.h"

class Receiver : public Actor
{
public:
	Receiver(){};

public:
	void ProcEvent(Act* act, DWORD bytes_transferred)
	{
		assert(dynamic_cast<TcpAct*>(act));
		TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);
		assert(tcpact.TcpSocket_);
		TcpSocket& tcpsocket = *tcpact.TcpSocket_;

		//printf("...Receiver (%d byte) s(%d)\n", bytes_transferred, tcpsocket.GetSocket() );

		// passive 연결끊김
		if (bytes_transferred == 0)
		{
			tcpsocket.Disconnect();
		}
		else
		{
			//printf("받은 값 = %s\n", tcpsocket.RecvBuf_);

			// Async Recv 포스트
			tcpsocket.Recv();

			//받은만큼 에코
			tcpsocket.Send((BYTE*)tcpsocket.RecvBuf_, bytes_transferred);
		}

	}

	void ProcError(Act* act, DWORD error)
	{
		assert(dynamic_cast<TcpAct*>(act));

		TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

		assert(tcpact.TcpSocket_);

		TcpSocket& tcpsocket = *tcpact.TcpSocket_;

		//printf("...에러처리 Receiver s(%d) err(%d)\n", tcpsocket.GetSocket(), error );

		tcpsocket.Disconnect();

	}

	void Init(Proactor* proactor)
	{
		Proactor_ = proactor;
	}

};