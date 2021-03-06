#include "stdafx.h"

void BeforeMakingIOCPMessage(const char* p_szStr)
{
#ifdef MYDEF
	if (0 > printf_s(p_szStr))
		printf_s("The OS has failed to write a message in debug mode before making IOCP.\n");
	system("pause");
	exit(1);
#else
	// If this state is the release mode, then make the beep sound thrice !!!
	if (0 > printf_s("\a\a\a"))
		printf_s("\a\a\a\a");
	system("pause");
	exit(1);
	// Kill This Service ! Don't use KillProcess or TerminateProcess !
#endif
}



CLogWriter::CLogWriter(PTCH p_szStr, int p_nNumOfThreads)
:m_nNumOfThreads(p_nNumOfThreads)
{
#ifdef MYDEF
#else
	m_pLogAct = new CLogAct(p_szStr);

	/*m_hLogIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, m_nNumOfThreads);

	if (NULL == m_hLogIOCP)
	{
		BeforeMakingIOCPMessage("The OS has failed to make a IOCP kernel object for the log file!\n");
	}

	for (int i = 0; i < m_nNumOfThreads; ++i)
	{
		unsigned int t_unThreadId;
		if (-1 == _beginthreadex(NULL, 0, CLogWriter::ThreadProc, (void *)this, 0, &t_unThreadId))
		{
			BeforeMakingIOCPMessage("The OS has failed to make a worker thread for log file!\n");
		}
	}

	ULONG_PTR t_pulData = 0;

	if (NULL == ::CreateIoCompletionPort(m_pLogAct->m_cFile.m_hFile, m_hLogIOCP, t_pulData, 0))
	{
		BeforeMakingIOCPMessage("The OS has failed to assign the log file to the IOCP !\n");
	}*/
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
	while (true){
		DWORD bytes_transferred;
		DWORD completionkey;

		OVERLAPPED* overlapped = NULL;


		//PRINT("thread ready\n");
		BOOL status = ::GetQueuedCompletionStatus(static_cast<CLogWriter*>(p_pRaram)->m_hLogIOCP, &bytes_transferred, static_cast<PULONG_PTR>(&completionkey), &overlapped, INFINITE);
		// Check The Finish of a Writing


		if (status == TRUE)
		{
			if (overlapped == NULL)
			{
				continue;
			}
		}
		else
		{
			BeforeMakingIOCPMessage("Proactor ProcEvents() status false Error!!! err\n");
			continue;
		}
	}
	
	return 0;
}

int CLogWriter::myPRINTF(LPTCH p_szStr, ...)
{
	char buf[1000];
	ZeroMemory(buf, 1000);

	va_list argList;
	va_start(argList, p_szStr);
	if (0 > vsprintf(buf, p_szStr, argList))
		BeforeMakingIOCPMessage("The LogWriter has failed to write a message in debug mode.\n");
	va_end(argList);

#ifdef MYDEF
	if (0 > printf_s(buf))
		BeforeMakingIOCPMessage("The LogWriter has failed to write a message in debug mode.\n");
	return 0;
#else
	DWORD			t_dwNumberOfBytesWritten;
	int t_nResult;

	// For Writing the Unicode Mark
	t_nResult = ::WriteFile(m_pLogAct->m_cFile.m_hFile, buf, strlen(buf), &t_dwNumberOfBytesWritten, NULL);


	if (!t_nResult)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			BeforeMakingIOCPMessage("The LogWriter has failed to write a message in release mode.\n");
		}
	}

	return t_nResult;
#endif
}