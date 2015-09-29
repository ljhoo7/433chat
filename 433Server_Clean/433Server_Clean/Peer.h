#pragma once

class IPeer
{
public:
	virtual bool recieveProcess() = 0;
	virtual void recieve_msg(char* buf, int size) = 0;
	virtual void send_msg(char* buf, int size) = 0;
	virtual void remove() = 0;
};