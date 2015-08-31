
#include <WinSock2.h>
#include <stdio.h>
#include <thread>
#include <list>

#include "usertoken.h";

class Reciever{

public:
	Reciever();
	~Reciever();

	void start(int port, int backlog, void(*callback)(UserToken& token));


private:
	void addUserList(UserToken user);
	void deleteUserList(UserToken user);


	void process();
	void recieveProcess();
	void acceptProcess();


public:


private:
	void(*callback)(UserToken& token);
	SOCKET listenSocket;
	SOCKADDR_IN serveraddr;

	FD_SET reads;
	std::list<UserToken> userList;

};