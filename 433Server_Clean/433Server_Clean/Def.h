#pragma once

typedef struct tagPerIoContext
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

typedef struct tagPerSocketContext
{

	SOCKET socket;
	PerIoContext recvContext;
	PerIoContext sendContext;

} PerSocketContext, *PPerSocketContext;