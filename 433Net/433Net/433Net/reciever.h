#pragma once
#include "utilities.h"

class Reciever{

public:
	Reciever();
	~Reciever();

	void start(int port, void(*callback)(UserToken* token));
	void process();

private:
	void addUserList(UserToken* user);
	void deleteUserList(UserToken* user);

	void recieveProcess();
	void acceptProcess();

private:
	void(*callback)(UserToken* token);
	SOCKET listenSocket;
	SOCKADDR_IN serveraddr;

	FD_SET reads;
	FD_SET copy_set;
	std::list<UserToken*> userList;
};