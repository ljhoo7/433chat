#include "stdafx.h"

extern SYSTEM_INFO		si;
extern CLogicHandle		logicHandle;

int KeepReading(SOCKETINFO *ptr, int cbTransferred)
{
	ptr->recv_wsabuf.buf = ptr->recv_buf + cbTransferred;
	ptr->recv_wsabuf.len -= cbTransferred;
	ptr->receivedBytes += cbTransferred;
	ptr->toReceiveBytes -= cbTransferred;

	DWORD flags = 0, recvbytes;
	int retval;

	if (ptr->recv_wsabuf.len > 0)
		retval = WSARecv(ptr->sock, &ptr->recv_wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
	else
		retval = 0;

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			err_quit("KeepReading ERROR_IO_PENDING");
		}
	}
	return retval;
}

int DecodingHeader(pkt_type type, SOCKETINFO *ptr, int cbTransferred)
{
	ptr->recv_wsabuf.buf = ptr->recv_buf + cbTransferred;
	int tLen;

	switch (type)
	{
	case pkt_type::pt_create:
		tLen = sizeof(t_create);
		break;
	case pkt_type::pt_destroy:
		tLen = sizeof(t_destroy);
		break;
	case pkt_type::pt_join:
		tLen = sizeof(t_join);
		break;
	case pkt_type::pt_leave:
		tLen = sizeof(t_leave);
		break;
	case pkt_type::pt_chat:
		tLen = HEADER_SIZE << 1;
		ptr->isChat = true;
		break;
	default:
		break;
	}
	
	ptr->recv_wsabuf.len = tLen - HEADER_SIZE;
	ptr->receivedBytes += cbTransferred;
	ptr->toReceiveBytes = ptr->recv_wsabuf.len;

	DWORD flags = 0, recvbytes;

	int retval;
	if (ptr->recv_wsabuf.len > 0)
		retval = WSARecv(ptr->sock, &ptr->recv_wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
	else
		retval = 0;

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			err_quit("DecodingHeader ERROR_IO_PENDING");
		}
	}
	return retval;
}

bool DecodingBody(SOCKETINFO *ptr, COMPLEMENT_KEY ck)
{
	if (ptr->toReceiveBytes == 0)
	{
		pkt_type _type = (pkt_type)((unsigned short)(*ptr->recv_buf));

		switch (_type)
		{
		case pkt_type::pt_create:
			{
				t_create *tCreate = (t_create*)ptr->recv_buf;
				printf("create %d\n", tCreate->room_num);
			}
			break;
		case pkt_type::pt_destroy:
			{
				t_destroy *tDestroy = (t_destroy*)ptr->recv_buf;
				printf("destroy %d\n", tDestroy->room_num);
			}
			break;
		case pkt_type::pt_join:
			{
				t_join *tJoin = (t_join*)ptr->recv_buf;
				printf("join %d %s\n", tJoin->room_num, tJoin->nickname);
			}
			break;
		case pkt_type::pt_leave:
			{
				t_leave *tLeave = (t_leave*)ptr->recv_buf;
				printf("leave %d %s\n", tLeave->room_num, tLeave->nickname);
			}
			break;
		case pkt_type::pt_chat:
			{
				int size = sizeof(unsigned short)+sizeof(unsigned short)+sizeof(unsigned short)
					+20 + sizeof(unsigned int);
				t_chat *tChat = new t_chat();
				memcpy(tChat, ptr->recv_buf, size);
				tChat->message = std::string(ptr->recv_buf, size);
			}
			break;
		default:
			break;
		}

		CPlayer *pPlayer = static_cast<CPlayer*>(ptr->pPeer);

		char* buf = pPlayer->poolManager.pop();
		CPacket* msg = pPlayer->packetPoolManager.pop();
		msg->type = 2;
		msg->owner = pPlayer->token;
		msg->msg = buf;
		memcpy(buf, ptr->recv_buf, BUFSIZE);
		logicHandle.enqueue_oper(msg, false);

		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ZeroMemory(ptr->recv_buf, BUFSIZE);
		ZeroMemory(ptr->send_buf, BUFSIZE);
		ptr->toSendBytes = 0;
		ptr->toReceiveBytes = HEADER_SIZE;
		ptr->sentBytes = ptr->receivedBytes = 0;
		ptr->recv_wsabuf.buf = ptr->recv_buf;
		ptr->recv_wsabuf.len = ptr->toReceiveBytes;
		ptr->send_wsabuf.buf = ptr->send_buf;
		ptr->send_wsabuf.len = ptr->toSendBytes;
		ptr->isChat = false;

		DWORD recvbytes, flags;

		flags = 0;

		int retval;
		if (ptr->recv_wsabuf.len > 0)
			retval = WSARecv(ptr->sock, &ptr->recv_wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
		else
			retval = 0;

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				err_quit("WSARecv() error on DecodingBody");
			}
		}
	}
	else if (ptr->toReceiveBytes < 0)
	{
		err_quit("There is an error on deserializing header !");
	}
	return true;
}

DWORD WINAPI WorkerThreadForClient(LPVOID arg)
{
	/*std::list<CUserToken*>::iterator iter;
	for (iter = userList.begin(); iter != userList.end(); iter++){
		if (!(*iter)->recieveProcess()){
			printf("closed client:%d\n", (*iter)->clientSocket);
			deleteUserList(*iter);
			break;
		}
	}*/
	int retval;

	HANDLE hcp = (HANDLE)arg;
	while (1)
	{
		DWORD cbTransferred;
		COMPLEMENT_KEY ck;
		SOCKETINFO *ptr;

		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, (PULONG_PTR)&ck, (LPOVERLAPPED*)&ptr, INFINITE);

		// Get the information of this client
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);

		if (retval == 0 || cbTransferred == 0)
		{
			if (retval == 0)
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped, &temp1, FALSE, &temp2);
				err_display("WSAGetOverlappedResult");
			}
			closesocket(ptr->sock);
			printf("[IOCP server] client closed : IP address = %s, port num = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			delete ptr;
			continue;
		}

		if (ptr->sock == NULL)
		{
			std::cout << "There is a weired I/O completion which has not connected socket." << std::endl;
			return false;
		}

		//------------------------------------------------------------

		if (ptr->toReceiveBytes > 0)					// This is a Completed Receiving I/O
		{
			if (ptr->toSendBytes > 0)
			{
				err_quit("error on worker thread - This I/O signal has receive and send both\n");
			}
			if (ptr->toReceiveBytes < HEADER_SIZE)
			{
				if (ptr->receivedBytes >= HEADER_SIZE)
				{
					// The received is non - header
					KeepReading(ptr, cbTransferred);
					DecodingBody(ptr, ck);
				}
				else
				{
					// The received is header
					KeepReading(ptr, cbTransferred);

					if (ptr->toReceiveBytes == 0)
					{
						pkt_type _type = (pkt_type)((unsigned short)(*ptr->recv_buf));
						DecodingHeader(_type, ptr, cbTransferred);
					}
					else if (ptr->toReceiveBytes < 0)
					{
						err_quit("There is an error on deserializing header !");
					}
				}
			}
			else if (cbTransferred == HEADER_SIZE)
			{
				if (ptr->receivedBytes >= HEADER_SIZE)
				{
					if (ptr->isChat)
					{
						// The received is the size of chat
						KeepReading(ptr, cbTransferred);

						t_chat *pChat = (t_chat*)ptr->recv_buf;
						
						ptr->recv_wsabuf.buf += cbTransferred;
						ptr->recv_wsabuf.len = pChat->length - (HEADER_SIZE << 1);
						ptr->receivedBytes += cbTransferred;
						ptr->toReceiveBytes = ptr->recv_wsabuf.len;

						DWORD flags = 0, recvbytes;

						int retval;
						if (ptr->recv_wsabuf.len > 0)
							retval = WSARecv(ptr->sock, &ptr->recv_wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
						else
							retval = 0;

						if (retval == SOCKET_ERROR)
						{
							if (WSAGetLastError() != ERROR_IO_PENDING)
							{
								err_quit("DecodingHeader ERROR_IO_PENDING");
							}
						}
						ptr->isChat = false;
					}
					else
					{
						// The received is non - header
						KeepReading(ptr, cbTransferred);
						DecodingBody(ptr, ck);
					}
				}
				else if (ptr->receivedBytes == 0)
				{
					// The received is exactly the header !
					pkt_type _type = (pkt_type)((unsigned short)(*ptr->recv_buf));
					DecodingHeader(_type, ptr, cbTransferred);
				}
				else
				{
					// The received = header remain + body fragment : very bad case
				}
			}
			else
			{
				if (ptr->receivedBytes >= HEADER_SIZE)
				{
					// The received is the body remain
					KeepReading(ptr, cbTransferred);
					DecodingBody(ptr, ck);
				}
				else
				{
					// The received = header remain + body fragment : very bad case
				}
			}

		}
		else if (ptr->toSendBytes > 0)			// This is a Completed Sending I/O
		{
			if (ptr->toReceiveBytes > 0)
			{
				err_quit("error on worker thread - This I/O signal has receive and send both\n");
			}

			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ZeroMemory(ptr->recv_buf, BUFSIZE);
			ZeroMemory(ptr->send_buf, BUFSIZE);
			ptr->sock = ck.sock;
			ptr->toSendBytes = ptr->toReceiveBytes = 0;
			ptr->sentBytes = ptr->receivedBytes = 0;
			ptr->recv_wsabuf.buf = ptr->recv_buf;
			ptr->recv_wsabuf.len = ptr->toReceiveBytes;
			ptr->send_wsabuf.buf = ptr->send_buf;
			ptr->send_wsabuf.len = ptr->toSendBytes;
			ptr->pPeer = NULL;
			ptr->isChat = false;
		}
		else
		{
			err_quit("error on worker thread\n");
		}
	}
	return true;
}

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

	// Create I/O Completion Port Kernel Object
	hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL)
	{
		err_quit("The iocp for client to client hasn't been created.");
		return;
	}

	// Create (CPU num * 2) Worker Threads
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++)
	{
		hThread = CreateThread(NULL, 0, WorkerThreadForClient, hcp, 0, NULL);
		if (hThread == NULL)
		{
			err_quit("A worker thread hasn't been created.");
			return;
		}
		CloseHandle(hThread);
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
	COMPLEMENT_KEY ck;
	int addrlen;
	SOCKADDR_IN clientaddr;
	DWORD recvbytes, flags;
	int retval;

	{
		addrlen = sizeof(clientaddr);
		ck.sock = accept(listenSocket, (struct sockaddr*)&clientaddr, &addrlen);

		CUserToken *user = new CUserToken(ck.sock, clientaddr, NULL);
		printf("connected client: %d\n", ck.sock);

		this->callback(user);
		addUserList(user);

		// Assignment the client socket to the iocp kernel object
		CreateIoCompletionPort((HANDLE)ck.sock, hcp, (ULONG_PTR)&ck, 0);

		SOCKETINFO *ptr = new SOCKETINFO;
		if (ptr == NULL)
		{
			err_quit("the assignment of SOCKEINFO has been failed.");
			return;
		}
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = ck.sock;
		ptr->toSendBytes = 0;
		ptr->toReceiveBytes = HEADER_SIZE;
		ptr->sentBytes = ptr->receivedBytes = 0;
		ptr->recv_wsabuf.buf = ptr->recv_buf;
		ptr->recv_wsabuf.len = ptr->toReceiveBytes;
		ptr->send_wsabuf.buf = ptr->send_buf;
		ptr->send_wsabuf.len = ptr->toSendBytes;
		ptr->pPeer = user->peer;
		ptr->isChat = false;

		flags = 0;
		retval = WSARecv(ck.sock, &ptr->recv_wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
		if (retval = SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				err_quit("WSARecv() error on CReceiver::acceptProcess");
			}
		}

		
	}

}

void CReceiver::recieveProcess()
{
	// IOCP replace this.
}

void CReceiver::addUserList(CUserToken* user)
{
	FD_SET(user->clientSocket, &reads);
	userList.push_back(user);
}


void CReceiver::deleteUserList(CUserToken* user)
{
	FD_CLR(user->clientSocket, &reads);
	userList.remove(user);
	user->remove();

	closesocket(user->clientSocket);

	delete user;
}