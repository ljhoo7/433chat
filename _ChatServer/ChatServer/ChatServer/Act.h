#pragma once

#include "stdafx.h"

class Act : public OVERLAPPED
{
public:
	Act()
	{
		hEvent = NULL;
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;

		Actor_ = NULL;
	}

	virtual void Complete(DWORD bytes_transferred) = 0;
	virtual void Error(DWORD error) = 0;

public:
	Actor*	Actor_;
};

