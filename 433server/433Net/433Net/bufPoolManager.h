#pragma once
#include "utilities.h"

class BufPoolManager{

public:
	BufPoolManager(int count);
	~BufPoolManager();
	char* pop();
	void push(char* a);

private:
	std::vector<char*> bufs;
};

class PacketPoolManager{

public:
	PacketPoolManager(int count);
	~PacketPoolManager();
	Packet* pop();
	void push(Packet* a);

private:
	std::vector<Packet*> bufs;
};