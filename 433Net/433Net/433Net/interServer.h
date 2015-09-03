#pragma once
#include "utilities.h"
#include "bufPoolManager.h"

extern LogicHandle logicHandle;

class InterServer{

public:
	SOCKET the_other_sock;
	int type;
	BufPoolManager poolManager;
	PacketPoolManager packetPoolManager;

	std::thread listen_thread;
	std::thread process_thread;
	std::thread heart_thread;

	SOCKET listen_sock;

private:
	bool beat_check;
	
public:
	InterServer();
	void listenProcess();
	void process();
	void recieveProcess();
	void disconnect();
	void packetHandling(Packet* packet);
	void heartbeat_check();
	void interserver_connect(char* ip, int port);
	void interserver_listen(int port);

	
	

	void recieve(char* buf, int size);
	void _send(char* buf, int size);

	void start(int type, int port);

};