#pragma once

#include "stdafx.h"

class Proactor
{
public:
	Proactor(int p_nThreadPoolSize);
	~Proactor();
public:
	void ProcEvents();

	void Register(HANDLE handle);

	static UINT WINAPI ThreadProc(void* pProactor);

public:
	HANDLE	m_hIOCP;
	int		m_nNumOfThreads;
};

