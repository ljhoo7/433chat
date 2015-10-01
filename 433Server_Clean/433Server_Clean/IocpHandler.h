#pragma once
class CIocpHandler
{
public:
	CIocpHandler();
	~CIocpHandler();

	bool Create(int numOfThreads, int* err);
	bool Associate(SOCKET socket, ULONG_PTR CompletionKey, int* err);
	BOOL GetCompletionStatus(ULONG_PTR* pOutCompletionKey, LPDWORD pdwOutBytesTransferred, LPOVERLAPPED* pOutOverlapped, int* pnOutErrCode = NULL, DWORD dwWaitingTime = INFINITE);
	bool CreateThreadPool(void* in, int nNumOfPooledThread, int type);

private:
	HANDLE hIOCP;
	int createdThreadNumber;


};

