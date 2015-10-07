#pragma once

#include "stdafx.h"

struct CLogFileInfo
{
	HANDLE m_hFile;

	PTCH		m_szFileWriteBuf[BUFSIZE];

	CLogFileInfo(PTCH p_szFileName)
	{

		m_hFile = ::CreateFileW(p_szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			printf("The OS Can't Create a Logfile !\n");
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