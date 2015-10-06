#pragma once

void BeforeMakingIOCPMessage(PTCH p_szStr)
{
#ifdef _DEBUG
	if (0 > wprintf_s(p_szStr))
		BeforeMakingIOCPMessage(L"The OS has failed to write a message in debug mode before making IOCP.\n");
	exit(1);
#else
	// If this state is the release mode, then make the beep sound thrice !!!
	if (0 > wprintf_s(L"\a\a\a"))
		BeforeMakingIOCPMessage(L"The OS has failed to make the beep sound in release mode before making IOCP.\n");
	// Kill This Service ! Don't use KillProcess or TerminateProcess !
#endif
}

struct CLogFileInfo
{
	HANDLE m_hFile;

	PTCH		m_szFileWriteBuf[BUFSIZE];

	CLogFileInfo(PTCH p_szFileName)
	{

		m_hFile = ::CreateFileW(p_szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			BeforeMakingIOCPMessage(L"The OS Can't Create a Logfile !\n");
		}

		ZeroMemory(m_szFileWriteBuf, BUFSIZE);
	}

	~CLogFileInfo()
	{
		CloseHandle(m_hFile);
	}
};

struct CLogAct : public OVERLAPPED
{
	CLogFileInfo m_cFile;

	CLogAct(PTCH p_szFileName)
		:m_cFile(p_szFileName)
	{
		hEvent = NULL;
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;
	}

	void Complete()
	{
	}
};