#pragma once
#include "utilities.h"

class UserToken{
public:
	SOCKET clientSoket;
	SOCKADDR_IN clientAddr;
	

	char buf[BUFSIZE];
	int position;
	short remainBytes;


public:
	UserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr);
	~UserToken();

	bool operator==(const UserToken& right);

private:
	void read_until();

public:
	void recieveProcess();
	void on_msg(char* buf);
};