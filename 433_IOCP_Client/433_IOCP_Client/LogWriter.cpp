#include "stdafx.h"

CLogWriter::CLogWriter(PTCH p_szStr, int p_nNumOfThreads)
:m_nNumOfThreads(p_nNumOfThreads)
{
#ifdef MYDEF
#else
	m_hLogIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, m_nNumOfThreads);
	
	if (NULL == m_hLogIOCP)
	{
		BeforeMakingIOCPMessage(L"The OS has failed to make a IOCP kernel object for the log file!\n");
	}

	for( int i = 0; i < m_nNumOfThreads; ++i)
	{
		unsigned int t_unThreadId;
		if (-1 == _beginthreadex(NULL, 0, CLogWriter::ThreadProc, (void *)this, 0, &t_unThreadId))
		{
			BeforeMakingIOCPMessage(L"The OS has failed to make a worker thread for log file!\n");
		}
	}

	m_pLogAct = new CLogAct(p_szStr);

	ULONG_PTR t_pulData = 0;

	if (NULL == ::CreateIoCompletionPort(m_pLogAct->m_cFile.m_hFile, m_hLogIOCP, t_pulData, 0))
	{
		BeforeMakingIOCPMessage(L"The OS has failed to assign the log file to the IOCP !\n");
	}

	unsigned short	t_usMark = 0xFEFF;
	DWORD			t_dwNumberOfBytesWritten;
	int t_nResult;

	// For Writing the Unicode Mark
	t_nResult = ::WriteFile(m_pLogAct->m_cFile.m_hFile, &t_usMark, sizeof(t_usMark), &t_dwNumberOfBytesWritten, m_pLogAct);

	if (!t_nResult)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			BeforeMakingIOCPMessage(L"The OS has failed to write the unicode mark on the log file !\n");
		}
	}
#endif
}

CLogWriter::~CLogWriter()
{
#ifdef MYDEF
#else
	CloseHandle(m_hLogIOCP);
	delete m_pLogAct;
#endif
}

UINT WINAPI CLogWriter::ThreadProc(PVOID p_pRaram)
{
	// Check The Finish of a Writing
	return 0;
}

int CLogWriter::myWprintf(LPTCH p_szStr, ...)
{
	wchar_t buf[1000];
	ZeroMemory(buf, 1000);

	va_list argList;
	va_start(argList, p_szStr);
	if (0 > vswprintf(buf, 1000, p_szStr, argList))
		BeforeMakingIOCPMessage(L"The LogWriter has failed to write a message in debug mode.\n");
	va_end(argList);

#ifdef MYDEF
	if (0 > wprintf_s(buf))
		BeforeMakingIOCPMessage(L"The LogWriter has failed to write a message in debug mode.\n");
	return 0;
#else
	DWORD			t_dwNumberOfBytesWritten;
	int t_nResult;

	// For Writing the Unicode Mark
	t_nResult = ::WriteFile(m_pLogAct->m_cFile.m_hFile, buf, wcslen(buf) * sizeof(TCHAR), &t_dwNumberOfBytesWritten, m_pLogAct);

	if (!t_nResult)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			BeforeMakingIOCPMessage(L"The LogWriter has failed to write a message in release mode.\n");
		}
	}

	return t_nResult;
#endif
}