#include <sys/types.h>
#include <WinSock2.h>
#include <stdio.h>

class UserToken{
public:
	SOCKET clientSoket;
	SOCKADDR_IN clientAddr;

	char buf[1024];

public:
	UserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr);
	~UserToken();

public:
	void recieveProcess();
};