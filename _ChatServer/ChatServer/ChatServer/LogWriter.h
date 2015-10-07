#pragma once

#include "LogAct.h"

class LogWriter
{
	HANDLE m_hLogIOCP;

	int m_nNumOfThreads;

	CLogAct *m_pLogAct;
public:
	static UINT WINAPI ThreadProc(PVOID p_pRaram);

	LogWriter(PTCH p_szStr, int p_nNumOfThreads);
	~LogWriter();

	int LogPrint(PTCH p_szStr);
};