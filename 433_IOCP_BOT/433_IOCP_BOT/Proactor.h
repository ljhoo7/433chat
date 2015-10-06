#pragma once

struct CProactor
{
	CProactor(int p_nThreadPoolSize);
	~CProactor();

	void ProcEvents();

	static UINT WINAPI ThreadProc(void* pProactor);

	HANDLE	m_hIOCP;
	int		m_nNumOfThreads;
};

