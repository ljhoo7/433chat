#pragma once

class CInterServer
{
	bool beat_check;
public:
	CInterServer();
	~CInterServer();

	SOCKET the_other_sock;

	int type;

	CBufPoolManager poolManager;
	CPacketPoolManager packetPoolManager;

	std::thread listen_thread;
	std::thread process_thread;
	std::thread heart_thread;

	SOCKET listen_sock;

	void makeSync();
	void listenProcess();
	void process();

	void workerThreadProcess();

	void _recieve(char* buf, int size);
	void temp_recieveProcess();
	void recieveProcess();
	void disconnect();
	void packetHandling(CPacket* packet);
	void heartbeat_check();
	void interserver_connect(char* ip, int port);
	void interserver_listen(int port);

	void recieve(char* buf, int size);
	void _send(char* buf, int size);

	void start(int type, int port);

private:
	CIocpHandler IocpHandler;
	PerSocketContext SocketContext;

	CRITICAL_SECTION disconnection_lock;

};