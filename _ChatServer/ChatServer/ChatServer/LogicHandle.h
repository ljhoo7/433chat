#pragma once

class CLogicHandle
{
	std::queue<CPacket *> operationQueue;
	std::queue<CPacket *> interOperationQueue;
	CRITICAL_SECTION operationLock;
	CRITICAL_SECTION interOperationLock;
public:
	CLogicHandle();
	~CLogicHandle();

	void EnqueueOper(CPacket* msg, bool interServer);
	void Process();

	bool isEnd;
};

