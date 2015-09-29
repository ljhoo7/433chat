#pragma once

class CReceiver
{
	void addUserList(CUserToken* user);
	void deleteUserList(CUserToken* user);

	void recieveProcess();
	void acceptProcess();

	void(*callback)(CUserToken* token);
	SOCKET listenSocket;
	SOCKADDR_IN serveraddr;

	FD_SET reads;
	FD_SET copy_set;
	std::list<CUserToken*> userList;

public:
	CReceiver();
	~CReceiver();

	void start(int port, void(*callback)(CUserToken* token));
	void process();
};

