#include "stdafx.h"

extern CInterServer listen_server;
extern CInterServer connect_server;

CLogicHandle::CLogicHandle()
{
	InitializeCriticalSection(&operation_lock);
	InitializeCriticalSection(&inter_operation_lock);
}
CLogicHandle::~CLogicHandle()
{
	DeleteCriticalSection(&operation_lock);
	DeleteCriticalSection(&inter_operation_lock);
}

void CLogicHandle::enqueue_oper(CPacket* msg, bool interServer)
{
	if (!interServer)
	{
		EnterCriticalSection(&operation_lock);
		operation_queue.push(msg);
		//_event.Set();
		LeaveCriticalSection(&operation_lock);
	}
	else
	{
		EnterCriticalSection(&inter_operation_lock);
		inter_operation_queue.push(msg);
		//_event.Set();
		LeaveCriticalSection(&inter_operation_lock);
	}

}

void CLogicHandle::process()
{
	while (true)
	{
		if (inter_operation_queue.size() <= 0)
		{
			if (operation_queue.size() <= 0)
			{
				continue;
				/* wait! */
				//_event.WaitOne();
			}
			CPacket* packet = nullptr;

			EnterCriticalSection(&operation_lock);
			if (operation_queue.size() > 0)
			{
				packet = operation_queue.front();
				operation_queue.pop();
			}
			LeaveCriticalSection(&operation_lock);

			if (packet != nullptr)
			{
				if (packet->type == 2)
				{
					CPlayer *tPlayer = static_cast<CPlayer *>(packet->owner->peer);
					tPlayer->packetHandling(packet);
				}
			}
			continue;
			/* wait! */
			//_event.WaitOne();
		}
		CPacket* packet = nullptr;

		EnterCriticalSection(&inter_operation_lock);
		if (inter_operation_queue.size() > 0)
		{
			packet = inter_operation_queue.front();
			inter_operation_queue.pop();
		}
		LeaveCriticalSection(&inter_operation_lock);

		if (packet != nullptr)
		{
			if (packet->type == 0)
			{
				connect_server.packetHandling(packet);
			}
			else if (packet->type == 1)
			{
				listen_server.packetHandling(packet);
			}
		}

	}
}
