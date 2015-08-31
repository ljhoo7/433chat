#include "reciever.h"
#include "utilities.h"

Reciever::Reciever(){
	this->callback = NULL;
}

Reciever::~Reciever(){

}

void Reciever::start(int port, int backlog, void(*callback)(UserToken& token)){
	int retval;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		err_quit("WSAStartup() error!");


	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) err_quit("socket()");

	
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);

	retval = bind(listenSocket, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	retval = listen(listenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	u_long on = 1;
	retval = ioctlsocket(listenSocket, FIONBIO, &on);
	if (retval == SOCKET_ERROR) err_display("ioctlsocket()");

	this->callback = callback;

	

	return;
}

void Reciever::process(){
	
	FD_ZERO(&reads);
	FD_SET(listenSocket, &reads);

	int retval;
	retval = select(0, &reads, 0, NULL, NULL);

	while (true){
		acceptProcess();
		recieveProcess();
	}
}

void Reciever::acceptProcess(){
	SOCKET clientSocket;
	int addrlen;
	SOCKADDR_IN clientaddr;

	if (FD_ISSET(listenSocket, &reads)){
		addrlen = sizeof(clientaddr);
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientaddr, &addrlen);
		UserToken *user = new UserToken(clientSocket, clientaddr);
		printf("connected client: %d\n", clientSocket);

		addUserList(*user);
		this->callback(*user);
	}
	
}
void Reciever::recieveProcess(){
	int retval;
	retval = select(0, &reads, 0, NULL, NULL);
	if (retval == SOCKET_ERROR){
		err_quit("select()");
		return;
	}
	if (retval == 0) return;
	std::list<UserToken>::iterator iter;
	for (iter = userList.begin(); iter != userList.end(); iter++){
		UserToken user = (*iter);
		if (FD_ISSET(user.clientSoket, &reads)){
			user.recieveProcess();
		}
	}

}

void Reciever::addUserList(UserToken user){
	FD_SET(user.clientSoket, &reads);
	userList.push_back(user);

}


void Reciever::deleteUserList(UserToken user){
	FD_CLR(user.clientSoket, &reads);
	userList.remove(user);
}