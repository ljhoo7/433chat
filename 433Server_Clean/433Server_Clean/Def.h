#pragma once

typedef struct tagPerIoContext_recieve
{

	WSAOVERLAPPED overlapped;
	WSABUF wsaBuf;
	char Buffer[BUFSIZE];
	int position;
	int remainBytes;
	BOOL isVar;
	//DWORD dwBytesTransferred; // �� IO ���۵� ����Ʈ �� 
	//DWORD dwTotalBytes;       // �� �󸶸�ŭ ���۵Ǿ����ĸ� �����ϴ� ����Ʈ �� 

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