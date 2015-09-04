#include "logic.h"
#include "interServer.h"
#include "packet.h"
#include "usertoken.h"
#include "player.h"

AutoResetEvent::AutoResetEvent(bool initial)
: flag_(initial)
{
}

void AutoResetEvent::Set()
{
	std::lock_guard<std::mutex> _(protect_);
	flag_ = true;
	signal_.notify_one();
}

void AutoResetEvent::Reset()
{
	std::lock_guard<std::mutex> _(protect_);
	flag_ = false;
}

bool AutoResetEvent::WaitOne()
{
	std::unique_lock<std::mutex> lk(protect_);
	while (!flag_) // prevent spurious wakeups from doing harm
		signal_.wait(lk);
	flag_ = false; // waiting resets the flag
	return true;
}

LogicHandle::LogicHandle(){
	InitializeCriticalSection(&operation_lock);
}
LogicHandle::~LogicHandle(){
	DeleteCriticalSection(&operation_lock);
}

void LogicHandle::enqueue_oper(Packet* msg){
	EnterCriticalSection(&operation_lock);
	operation_queue.push(msg);
	//_event.Set();
	LeaveCriticalSection(&operation_lock);
}

void LogicHandle::process(){
	while (true){
		if (operation_queue.size() <= 0){
			continue;
			/* wait! */
			//_event.WaitOne();
		}
		Packet* packet = nullptr;

		EnterCriticalSection(&operation_lock);
		if (operation_queue.size() > 0){
			packet = operation_queue.front();
			operation_queue.pop();
		}
		LeaveCriticalSection(&operation_lock);

		if (packet != nullptr){
			if (packet->type == 0){
				connect_server.packetHandling(packet);
			}
			else if (packet->type == 1){
				listen_server.packetHandling(packet);
			}
			else if (packet->type == 2){
				Player *tPlayer = static_cast<Player *>(packet->owner->peer);
				tPlayer->packetHandling(packet);
			}
		}
	}
}
