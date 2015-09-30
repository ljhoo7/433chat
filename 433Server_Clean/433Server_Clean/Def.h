#pragma once

typedef struct tagPerIoContext
{

	WSAOVERLAPPED overlapped;
	WSABUF wsaBuf;
	char Buffer[BUFSIZE];
	int position;
	int remainBytes;
	BOOL isVar;
	//DWORD dwBytesTransferred; // 현 IO 전송된 바이트 수 
	//DWORD dwTotalBytes;       // 총 얼마만큼 전송되었느냐를 저장하는 바이트 수 

} PerIoContext, *PPerIoContext;

typedef struct tagPerSocketContext
{

	SOCKET socket;
	PerIoContext recvContext;
	PerIoContext sendContext;

} PerSocketContext, *PPerSocketContext;