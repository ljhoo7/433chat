#include "stdafx.h"

extern ChatServer* chatServer;

CLogicHandle::CLogicHandle()
{
	isEnd = false;
	InitializeCriticalSection(&operationLock);
	InitializeCriticalSection(&interOperationLock);
}
CLogicHandle::~CLogicHandle()
{
	DeleteCriticalSection(&operationLock);
	DeleteCriticalSection(&interOperationLock);
}

void CLogicHandle::EnqueueOper(CPacket* msg, bool interServer)
{
	if (!interServer)
	{
		EnterCriticalSection(&operationLock);
		operationQueue.push(msg);
		//_event.Set();
		LeaveCriticalSection(&operationLock);
	}
	else
	{
		EnterCriticalSection(&interOperationLock);
		interOperationQueue.push(msg);
		//_event.Set();
		LeaveCriticalSection(&interOperationLock);
	}

}

void CLogicHandle::Process()
{
	while (!isEnd)
	{
		if (interOperationQueue.size() <= 0)
		{
			if (operationQueue.size() <= 0)
			{
				continue;
				/* wait! */
				//_event.WaitOne();
			}
			CPacket* packet = nullptr;

			EnterCriticalSection(&operationLock);
			if (operationQueue.size() > 0)
			{
				packet = operationQueue.front();
				operationQueue.pop();
			}
			LeaveCriticalSection(&operationLock);

			if (packet != nullptr)
			{
				if (packet->isAgent){
					AgentSocket *socket = static_cast<AgentSocket *>(packet->owner);
					socket->PacketHandling(packet);
				}
				else{
					CPlayer *tPlayer = static_cast<CPlayer *>(packet->owner);
					tPlayer->PacketHandling(packet);
				}
			}
			continue;
			/* wait! */
			//_event.WaitOne();
		}
		CPacket* packet = nullptr;

		EnterCriticalSection(&interOperationLock);
		if (interOperationQueue.size() > 0)
		{
			packet = interOperationQueue.front();
			interOperationQueue.pop();
		}
		LeaveCriticalSection(&interOperationLock);

		if (packet != nullptr)
		{
			InterSocket *socket = static_cast<InterSocket *>(packet->owner);
			socket->packetHandling(packet);
		}

	}

	return;
}
