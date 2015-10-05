#pragma once

#include "stdafx.h"

class TcpAct : public Act
{
public:
	TcpAct()
	{
		TcpSocket_ = NULL;
	}

	void Complete(DWORD bytes_transferred)
	{
		Actor_->ProcEvent(this, bytes_transferred);
	}

	void Error(DWORD error)
	{
		Actor_->ProcError(this, error);
	}

	void Init(Actor* actor, TcpSocket* tcpsocket)
	{
		Actor_ = actor;
		TcpSocket_ = tcpsocket;
	}

	const TcpSocket* GetTcpSocket()
	{
		return TcpSocket_;
	}

public:
	TcpSocket* TcpSocket_;
};

