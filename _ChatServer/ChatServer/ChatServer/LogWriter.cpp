#include "stdafx.h"

LogWriter::LogWriter(PTCH p_szStr, int p_nNumOfThreads)
:m_nNumOfThreads(p_nNumOfThreads)
{
	m_hLogIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, m_nNumOfThreads);
	
	if (NULL == m_hLogIOCP)
	{
		printf("The OS has failed to make a IOCP kernel object for the log file!\n");
	}

	for( int i = 0; i < m_nNumOfThreads; ++i)
	{
		unsigned int t_unThreadId;
		if (-1 == _beginthreadex(NULL, 0, LogWriter::ThreadProc, (void *)this, 0, &t_unThreadId))
		{
			printf("The OS has failed to make a worker thread for log file!\n");
		}
	}

	m_pLogAct = new CLogAct(p_szStr);

	if (NULL == ::CreateIoCompletionPort(m_pLogAct->m_cFile.m_hFile, m_hLogIOCP, 0, 0))
	{
		printf("The OS has failed to assign the log file to the IOCP !\n");
	}

	unsigned short	t_usMark = 0xFEFF;
	DWORD			t_dwNumberOfBytesWritten;
	int t_nResult;

	// For Writing the Unicode Mark
	t_nResult = ::WriteFile(m_pLogAct->m_cFile.m_hFile, &t_usMark, sizeof(t_usMark), &t_dwNumberOfBytesWritten, m_pLogAct);

	if (!t_nResult)
	{
		printf("The OS has failed to write the unicode mark on the log file !\n");
	}
}

LogWriter::~LogWriter()
{
	CloseHandle(m_hLogIOCP);
	delete m_pLogAct;
}

UINT WINAPI LogWriter::ThreadProc(PVOID p_pRaram)
{
	// Check The Finish of an Writing
	return 0;
}

int LogWriter::LogPrint(PTCH p_szStr)
{
#ifdef _DEBUG
	if(0 > wprintf_s(p_szStr))
		printf("The LogWriter has failed to write a message in debug mode.\n");
	exit(1);
#else
	DWORD			t_dwNumberOfBytesWritten;
	int t_nResult;

	// For Writing the Unicode Mark
	t_nResult = ::WriteFile(m_pLogAct->m_cFile.m_hFile, p_szStr, wcslen(p_szStr) * sizeof(TCHAR), &t_dwNumberOfBytesWritten, m_pLogAct);

	if (!t_nResult)
	{
		BeforeMakingIOCPMessage(L"The LogWriter has failed to write a message in release mode.\n");
	}
#endif
}