#pragma once

struct CLogFileInfo
{
	HANDLE m_hFile;

	PTCH		m_szFileWriteBuf[BUFSIZE];

	CLogFileInfo(PTCH p_szFileName)
	{
#ifdef MYDEF
#else
		m_hFile = ::CreateFileW(p_szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			BeforeMakingIOCPMessage(L"The OS Can't Create a Logfile !\n");
		}

		ZeroMemory(m_szFileWriteBuf, BUFSIZE);
#endif
	}

	~CLogFileInfo()
	{
#ifdef MYDEF
#else
		CloseHandle(m_hFile);
#endif
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