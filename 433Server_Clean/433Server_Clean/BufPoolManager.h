#pragma once

class CBufPoolManager
{
	std::vector<char*> bufs;
public:
	CBufPoolManager(int count);
	~CBufPoolManager();

	char* pop();
	void push(char* a);
};

