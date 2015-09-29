#pragma once

class CPacketPoolManager
{
	std::vector<CPacket*> bufs;
public:
	CPacketPoolManager(int count);
	~CPacketPoolManager();

	CPacket* pop();
	void push(CPacket* a);
};