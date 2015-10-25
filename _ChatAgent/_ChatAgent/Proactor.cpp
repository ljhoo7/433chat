#include "stdafx.h"


UINT WINAPI Proactor::ThreadProc(void* pProactor)
{
	static_cast<Proactor*>(pProactor)->ProcEvents();

	return 0;
}

void Proactor::Register(HANDLE pHandle)
{
	CreateIoCompletionPort(pHandle, m_hIOCP, 0, 0);
}

Proactor::Proactor(int p_nThreadPoolSize)
:m_nNumOfThreads(p_nThreadPoolSize)
{
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, p_nThreadPoolSize);

	if (NULL == m_hIOCP)
	{
		PRINTF("CreateIoCompletionPort Create error !");
		exit(1);
	}

	for (int k = 0; k < p_nThreadPoolSize; ++k)
	{
		unsigned int t_unThreadId;
		_beginthreadex(NULL, 0, Proactor::ThreadProc, (void*)this, 0, &t_unThreadId);
	}
}

Proactor::~Proactor()
{
}

void Proactor::ProcEvents()
{
	while (1)
	{
		DWORD t_dwBytesTransferred;
		DWORD t_dwCompletionKey;

		OVERLAPPED *t_pOverlapped = NULL;

		BOOL t_bStatus = ::GetQueuedCompletionStatus(m_hIOCP, &t_dwBytesTransferred, static_cast<PULONG_PTR>(&t_dwCompletionKey), &t_pOverlapped, INFINITE);

		if (TRUE == t_bStatus)
		{
			if (NULL == t_pOverlapped)
			{
				PRINTF("Proactor's ProcEvents error !\n");
				continue;
			}

			Act *t_pAct = static_cast<Act*>(t_pOverlapped);
			t_pAct->Complete(t_dwBytesTransferred);
		}
		else
		{
			DWORD t_dwError = WSAGetLastError();
			if (NULL != t_pOverlapped)
			{
				Act *t_pAct = static_cast<Act*>(t_pOverlapped);
				//t_pAct->Error(t_dwByteesTransferred);
				continue;
			}
			PRINTF("Proactor's ProcEvents false error !\n");
			continue;
		}
	}
}