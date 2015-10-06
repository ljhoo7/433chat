#pragma once

#include "stdafx.h"

class Actor
{
public:
	Actor()
	{
		Proactor_ = NULL;
	}

	virtual void ProcEvent(Act* act, DWORD bytes_transferred) = 0;
	virtual void ProcError(Act* act, DWORD error) = 0;

public:
	Proactor* Proactor_;

};