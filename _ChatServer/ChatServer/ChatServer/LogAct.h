#pragma once

struct CLogFileInfo
{
	HANDLE m_hFile;

	PTCH		m_szFileWriteBuf[BUFSIZE];

	CLogFileInfo(PTCH p_szFileName)
	{
#ifdef MYDEF
#else
		m_hFile = ::CreateFile(p_szFileName,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			0, // | FILE_FLAG_NO_BUFFERING °¡´É
			NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			return;
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