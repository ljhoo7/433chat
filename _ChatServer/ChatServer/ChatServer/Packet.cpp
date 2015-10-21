#include "stdafx.h"

CPacket::CPacket()
{
	owner = NULL;
	msg = NULL;
	isAgent = false;
}

CPacket::CPacket(TcpSocket* owner, char* msg)
{
	this->owner = owner;
	this->msg = msg;
}

CPacket::~CPacket()
{
}
