#include "stdafx.h"

extern CLogWriter *g_pLog;

UINT WINAPI CProactor::ThreadProc(void* pProactor)
{
	static_cast<CProactor*>(pProactor)->ProcEvents();

	return 0;
}

CProactor::CProactor(int p_nThreadPoolSize)
:m_nNumOfThreads(p_nThreadPoolSize)
{
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, p_nThreadPoolSize);

	for (int k = 0; k < p_nThreadPoolSize; ++k)
	{
		unsigned int t_unThreadId;
		_beginthreadex(NULL, 0, CProactor::ThreadProc, (void*)this, 0, &t_unThreadId);
	}
}

CProactor::~CProactor()
{
}

void CProactor::ProcEvents()
{
	while (1)
	{
		DWORD t_dwBytesTransferred;
		DWORD t_dwCompletionKey;

		OVERLAPPED *t_pOverlapped = NULL;

		BOOL t_bStatus = ::GetQueuedCompletionStatus(m_hIOCP, &t_dwBytesTransferred, static_cast<PULONG_PTR>(&t_dwCompletionKey), &t_pOverlapped, INFINITE);

		if (true == t_bStatus)
		{
			if (NULL == t_pOverlapped)
			{
				g_pLog->myWprintf(L"Proactor's ProcEvents error !\n");
				continue;
			}

			CAct *t_pAct = static_cast<CAct*>(t_pOverlapped);
			t_pAct->Complete(t_dwByteesTransferred);
		}
		else
		{
			DWORD t_dwError = WSAGetLastError();
			if (NULL != t_pOverlapped)
			{
				CAct *t_pAct = static_cast<CAct*>(t_pOverlapped);
				t_pAct->Error(t_dwByteesTransferred);
				continue;
			}
			g_pLog->myWprintf(L"Proactor's ProcEvents false error !\n");
			continue;
		}
	}
}