#pragma once

#include "stdafx.h"

class Proactor
{
public:
	Proactor();

public:
	static UINT	WINAPI ThreadProc(void* pProactor);

public:
	void Init(int numofthreads);
	void Register(HANDLE handle);
	void PostPrivateEvent(DWORD pId, Act* pActor);
	void ProcEvents();

public:
	HANDLE	iocp_;

	DWORD	timeOut_;
	DWORD	numOfThreads_;

};

