#pragma once
class CIocpHandler
{
public:
	CIocpHandler();
	~CIocpHandler();

	bool Create(int numOfThreads, int* err);
	bool Associate(SOCKET socket, ULONG_PTR CompletionKey, int* err);
	BOOL GetCompletionStatus(ULONG_PTR* pOutCompletionKey, LPDWORD pdwOutBytesTransferred, LPOVERLAPPED* pOutOverlapped, int* pnOutErrCode = NULL, DWORD dwWaitingTime = INFINITE);
	bool CreateThreadPool(CInterServer* in, int nNumOfPooledThread);

private:
	HANDLE hIOCP;
	int createdThreadNumber;


};

