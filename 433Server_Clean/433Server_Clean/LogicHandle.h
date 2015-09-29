#pragma once

class CLogicHandle
{
	std::queue<CPacket *> operation_queue;
	std::queue<CPacket *> inter_operation_queue;
	CRITICAL_SECTION operation_lock;
	CRITICAL_SECTION inter_operation_lock;
public:
	CLogicHandle();
	~CLogicHandle();

	void enqueue_oper(CPacket* msg, bool interServer);
	void process();
};

