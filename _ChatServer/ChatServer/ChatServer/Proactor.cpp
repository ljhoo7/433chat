#include "Proactor.h"

Proactor::Proactor()
{

}

UINT WINAPI Proactor::ThreadProc(void* pProactor)
{
	static_cast<Proactor*>(pProactor)->ProcEvents();

	return 0;
}

void Proactor::Init(int numofthreads)
{
	TimeOut_ = INFINITE;
	NumOfThreads_ = numofthreads;

	Iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, numofthreads);

	for (int i = 0; i<numofthreads; i++)
	{
		unsigned int threadid;
		_beginthreadex(NULL, 0, Proactor::ThreadProc, (void *)this, 0, &threadid);
	}
}

void Proactor::Register(HANDLE pHandle)
{
	::CreateIoCompletionPort(pHandle, Iocp_, 0, 0);
}

void Proactor::PostPrivateEvent(DWORD pId, Act* act)
{
	BOOL ret = ::PostQueuedCompletionStatus(Iocp_, 0, pId, act);
}

void Proactor::ProcEvents()
{

	while (TRUE)
	{
		DWORD bytes_transferred;
		DWORD completionkey;

		OVERLAPPED* overlapped = NULL;

		BOOL status = ::GetQueuedCompletionStatus(Iocp_, &bytes_transferred, static_cast<PULONG_PTR>(&completionkey), &overlapped, TimeOut_);

		if (status == TRUE)
		{
			if (overlapped == NULL)
			{
				printf("Proactor ProcEvents() Ov NULL Error!!! err(%d)\n", WSAGetLastError());
				continue;
			}
			// Async Completion Token 패턴 이용!
			Act* Actkn = static_cast<Act*>(overlapped);
			Actkn->Complete(bytes_transferred);
		}
		else
		{
			DWORD error = WSAGetLastError();

			if (overlapped != NULL)
			{
				Act* Actkn = static_cast<Act*>(overlapped);
				Actkn->Error(error);
				continue;
			}

			printf("Proactor ProcEvents() status false Error!!! err(%d)\n", error);
			continue;
		}

	}

}