#include "stdafx.h"

extern CInterServer listen_server;

CIocpHandler::CIocpHandler()
{
	hIOCP = NULL;
}


CIocpHandler::~CIocpHandler()
{
	if (hIOCP) CloseHandle(hIOCP);
}

bool CIocpHandler::Create(int numOfThreads, int* err){
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numOfThreads);
	if (hIOCP == NULL &&  err != NULL){
		*err = GetLastError();
	}
	return (hIOCP != NULL);
}

bool CIocpHandler::Associate(SOCKET socket, ULONG_PTR CompletionKey, int* err){
	if (socket == INVALID_SOCKET){
		err_quit("invalid socket!");
	}

	HANDLE retHandle = CreateIoCompletionPort((HANDLE)socket, hIOCP, CompletionKey, 0);
	if (retHandle != hIOCP && err != NULL)
	{
		*err = GetLastError();
	}

	return (retHandle == hIOCP);
}

BOOL CIocpHandler::GetCompletionStatus(ULONG_PTR* pOutCompletionKey, LPDWORD pdwOutBytesTransferred, LPOVERLAPPED* pOutOverlapped, int* pnOutErrCode,  DWORD dwWaitingTime){
	BOOL ret = GetQueuedCompletionStatus(hIOCP, pdwOutBytesTransferred, pOutCompletionKey, pOutOverlapped, dwWaitingTime);
	if (!ret && pnOutErrCode != NULL)
	{
		*pnOutErrCode = GetLastError();
	}

	return ret;
}
unsigned WINAPI ThreadFunction(void* arg)
{
	CInterServer* i = (CInterServer *)arg;
	i->workerThreadProcess();

	return 0;
}

bool CIocpHandler::CreateThreadPool(CInterServer* in, int nNumOfPooledThread){
	if (nNumOfPooledThread < 0){
		err_quit("num of threads >= 0 !");
	}

	if (nNumOfPooledThread == 0){
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		createdThreadNumber = si.dwNumberOfProcessors * 2;

	}
	else{
		createdThreadNumber = nNumOfPooledThread;
	}

	for (int i = 0; i < createdThreadNumber; i++){
		unsigned int dwThreadID = 0;
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, ThreadFunction, (void *)in, 0, &dwThreadID));
	}
	return true;
}

