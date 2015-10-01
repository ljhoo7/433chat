#pragma once

class CReceiver
{
	void recieveProcess();
	void acceptProcess();

	void(*callback)(CUserToken* token);
	SOCKET listenSocket;
	SOCKADDR_IN serveraddr;

public:
	CReceiver();
	~CReceiver();

	void start(int port, void(*callback)(CUserToken* token));
	void process();
	void workerThreadProcess();


private:
	void deleteUser(CUserToken* user);

	CIocpHandler IocpHandler;

};

