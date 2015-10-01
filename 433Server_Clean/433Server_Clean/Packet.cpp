#include "stdafx.h"

CPacket::CPacket()
{
	type = 0;
	owner = NULL;
	msg = NULL;
}

CPacket::CPacket(CUserToken* owner, char* msg, int type)
{
	this->owner = owner;
	this->msg = msg;
	this->type = type;
}

CPacket::~CPacket()
{
}
