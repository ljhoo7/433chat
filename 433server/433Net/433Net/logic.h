#pragma once
#include "utilities.h"

extern InterServer listen_server;
extern InterServer connect_server;

#include <mutex>
#include <condition_variable>
#include <thread>
#include <stdio.h>

class AutoResetEvent
{
public:
	explicit AutoResetEvent(bool initial = false);

	void Set();
	void Reset();

	bool WaitOne();

private:
	AutoResetEvent(const AutoResetEvent&);
	AutoResetEvent& operator=(const AutoResetEvent&); // non-copyable
	bool flag_;
	std::mutex protect_;
	std::condition_variable signal_;
};

class LogicHandle{
private:
	std::queue<Packet *> operation_queue;
	CRITICAL_SECTION operation_lock;
	AutoResetEvent _event;

public:
	LogicHandle();
	~LogicHandle();

	void enqueue_oper(Packet* msg);
	void process();
};