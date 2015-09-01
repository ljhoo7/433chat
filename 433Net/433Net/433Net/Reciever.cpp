#include "reciever.h"
#include "utilities.h"

extern int g_nIsListen;

Reciever::Reciever(){
	this->callback = NULL;
}

Reciever::~Reciever(){

}

void Reciever::start(int port, int backlog, void(*callback)(UserToken* token)){
	printf("start\n");

	int retval;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		err_quit("WSAStartup() error!");

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) err_quit("socket()");
	
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (g_nIsListen)
		serveraddr.sin_port = htons(SERVERPORT1);
	else
		serveraddr.sin_port = htons(SERVERPORT2);

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

	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	long long time;
	int fps = 30;
	double block = 1000 / fps;

	
	while (true){
		copy_set = reads;


		int retval;
		retval = select(0, &copy_set, 0, NULL, NULL);
		if (retval == SOCKET_ERROR){
			err_quit("select()");
			return;
		}
		if (retval == 0) continue;


		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		
		if (block <= time){
			acceptProcess();
			UserToken a = userList.front();
			recieveProcess();

			start_time = std::chrono::system_clock::now();
		}
		
	}
}

void Reciever::acceptProcess(){
	SOCKET clientSocket;
	int addrlen;
	SOCKADDR_IN clientaddr;

	if (FD_ISSET(listenSocket, &copy_set)){
		addrlen = sizeof(clientaddr);
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientaddr, &addrlen);
		UserToken *user = new UserToken(clientSocket, clientaddr, NULL);
		printf("connected client: %d\n", clientSocket);

		this->callback(user);
		addUserList(*user);
		

	}
	
}
void Reciever::recieveProcess(){
	std::list<UserToken>::iterator iter;
	for (iter = userList.begin(); iter != userList.end(); iter++){
		
		if (FD_ISSET(iter->clientSocket, &copy_set)){

			if (!iter->recieveProcess()){
				UserToken user = *iter;
				deleteUserList(*iter);
				printf("closed client:%d\n", user.clientSocket);
				
				break;
			}
		}
	}

}

void Reciever::addUserList(UserToken user){
	FD_SET(user.clientSocket, &reads);
	userList.push_back(user);

}


void Reciever::deleteUserList(UserToken user){
	FD_CLR(user.clientSocket, &reads);
	userList.remove(user);
	user.remove();
	closesocket(user.clientSocket);
	delete this;
}