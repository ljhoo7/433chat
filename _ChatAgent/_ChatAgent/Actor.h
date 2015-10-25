#pragma once
#include "stdafx.h"

class Actor
{
protected:
	Proactor		*mProactor;
public:
	Actor();
	virtual ~Actor();

	virtual void ProcEvent(Act *p_pAct, DWORD p_dwTransferredBytes) = 0;
	virtual void ProcError(Act *p_pAct, DWORD p_dwError) = 0;
};

