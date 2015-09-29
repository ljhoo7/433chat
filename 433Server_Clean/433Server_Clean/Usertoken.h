#pragma once

class CUserToken
{
	bool read_until();
	void start_send();
public:
	CUserToken();
	CUserToken(SOCKET clientSocket);
	CUserToken(SOCKET clientSocket, SOCKADDR_IN clientAddr, IPeer* peer);
	~CUserToken();

	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;

	char buf[BUFSIZE];
	int position;
	int remainBytes;
	bool var;
	IPeer* peer;

	bool operator==(const CUserToken& right);

	bool recieveProcess();
	void on_msg(char* buf, int size);
	void send_msg(char *buf, int len);
	void remove();
};

