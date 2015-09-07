#pragma once
#include "utilities.h"

class IPeer{
public:
	virtual bool recieveProcess() = 0;
	virtual void recieve_msg(char* buf, int size) = 0;
	virtual void send_msg(char* buf, int size) = 0;
	virtual void remove() = 0;
};

class UserToken{
public:
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;

	char buf[BUFSIZE];
	int position;
	int remainBytes;
	bool var;
	IPeer* peer;

public:
	UserToken(){}
	UserToken(SOCKET clientSocket);
	UserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr, IPeer* peer);
	~UserToken();

	bool operator==(const UserToken& right);

private:
	bool read_until();
	void start_send();

public:
	bool recieveProcess();
	void on_msg(char* buf, int size);
	void send_msg(char *buf, int len);
	void remove();
};