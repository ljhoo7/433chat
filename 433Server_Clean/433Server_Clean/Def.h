#pragma once

typedef struct tagPerIoContext_recieve
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


typedef struct tagPerIoContext_send
{

	WSAOVERLAPPED overlapped;
	WSABUF* wsaBuf;
} PerIoContext_send, *PPerIoContext_send;

typedef struct tagPerSocketContext
{

	SOCKET socket;
	PerIoContext recvContext;
	PerIoContext_send sendContext;

} PerSocketContext, *PPerSocketContext;

typedef struct tagPerSocketContext_userToken
{
	CUserToken* token;
	PerIoContext recvContext;
	PerIoContext_send sendContext;
} PerUserContext, *PPerUserContext;