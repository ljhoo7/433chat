#include "stdafx.h"

CReceiver::CReceiver()
{
	this->callback = NULL;
}


CReceiver::~CReceiver()
{
}

void CReceiver::start(int port, void(*callback)(CUserToken* token))
{
	printf("chatting server start\n");

	int ErrCode = 0;

	if (!IocpHandler.Create(0, &ErrCode)){
		printf("error code : %d\n", ErrCode);
		err_quit("create IO Completion port error");
	}
	if (!IocpHandler.CreateThreadPool((void *)this, 0, 1)){
		err_quit("Create Thread Pool Failed");
	}


	int retval;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		err_quit("WSAStartup() error!");

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) err_quit("socket()");

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	retval = bind(listenSocket, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	retval = listen(listenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	this->callback = callback;
	return;
}

void CReceiver::process()
{
	while (true){
		acceptProcess();
	}
}

void CReceiver::acceptProcess()
{
	SOCKET clientSocket;
	int addrlen;
	SOCKADDR_IN clientaddr;

	addrlen = sizeof(clientaddr);
	clientSocket = accept(listenSocket, (struct sockaddr*)&clientaddr, &addrlen);
	CUserToken *user = new CUserToken(clientSocket, clientaddr, NULL);
	printf("connected client: %d\n", clientSocket);

	this->callback(user);

	int ErrCode;

	if (!IocpHandler.Associate(clientSocket, reinterpret_cast<ULONG_PTR>(&user->SocketContext), &ErrCode))
	{
		printf("iocp associate error %d", ErrCode);
		err_quit("iocp associate()");
	}

	user->_recieve(user->SocketContext.recvContext.Buffer, HEADER_SIZE);
}
void CReceiver::recieveProcess()
{

/*	std::list<CUserToken*>::iterator iter;
	for (iter = userList.begin(); iter != userList.end(); iter++){
		if (FD_ISSET((*iter)->clientSocket, &copy_set)){
			if (!(*iter)->recieveProcess()){
				printf("closed client:%d\n", (*iter)->clientSocket);
				deleteUserList(*iter);
				break;
			}
		}
	}*/
}

void CReceiver::deleteUser(CUserToken* user)
{
	printf("closed client:%d\n", user->clientSocket);
	user->remove();
	closesocket(user->clientSocket);
	delete user;
}

void CReceiver::workerThreadProcess(){
	PPerUserContext user = NULL;
	PPerIoContext pPerIoCtx = NULL;
	DWORD dwBytesTransferred = 0;
	int ErrCode = 0;

	while (TRUE)
	{
		BOOL bRet = IocpHandler.GetCompletionStatus(reinterpret_cast<ULONG_PTR*>(&user),
			&dwBytesTransferred,
			reinterpret_cast<LPOVERLAPPED*>(&pPerIoCtx),
			&ErrCode);

		if (!bRet || dwBytesTransferred == 0){
		/*	if (!bRet)
			{
				WSAGetOverlappedResult(user->token->clientSocket, &pPerIoCtx->overlapped, NULL, FALSE, NULL);
				//err_display("WSAGetOverlappedResult");
			}*/

			printf("Client Connection Close, Socket will Close.\n");
			deleteUser(user->token);
			return;
			//err_quit("interserver iocp recieve complete error!");
		}

		if (pPerIoCtx == &user->token->SocketContext.recvContext){
			user->token->SocketContext.recvContext.position += dwBytesTransferred;
			user->token->SocketContext.recvContext.remainBytes -= dwBytesTransferred;
			user->token->recieveProcess();
		}
		else if ((PPerIoContext_send)pPerIoCtx == &user->token->SocketContext.sendContext){
			/* sendProcess */
			//user->sendProcess();
		}
		else{
			printf("interserver Send Context error.\n");
			deleteUser(user->token);
			return;
		}
	}
}