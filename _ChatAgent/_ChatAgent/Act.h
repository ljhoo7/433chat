#pragma once

#include "stdafx.h"

class CAct : public OVERLAPPED
{
public:
	CAct()
	{
		hEvent = NULL;
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;

		actor_ = NULL;
	}

	virtual void Complete(DWORD bytes_transferred) = 0;
	virtual void Error(DWORD error) = 0;

public:
	CActor*	actor_;
};

