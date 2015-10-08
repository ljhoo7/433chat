#pragma once

#include "LogAct.h"

class CLogWriter
{
	HANDLE m_hLogIOCP;

	int m_nNumOfThreads;

	CLogAct *m_pLogAct;
public:
	static UINT WINAPI ThreadProc(PVOID p_pRaram);

	CLogWriter(PTCH p_szStr, int p_nNumOfThreads);
	~CLogWriter();

	int myWPRINTF(LPTCH p_szStr,...);
};