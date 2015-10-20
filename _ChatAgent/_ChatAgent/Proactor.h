#pragma once

#include "stdafx.h"

struct CProactor
{
	CProactor(int p_nThreadPoolSize);
	~CProactor();

	void ProcEvents();

	void Register(HANDLE handle);

	static UINT WINAPI ThreadProc(void* pProactor);

	HANDLE	m_hIOCP;
	int		m_nNumOfThreads;
};

