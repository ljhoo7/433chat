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

	void workerThreadProcess();

	void _recieve(char* buf, int size);
	void recieveProcess(); // recieve complete
	void sendProcess(); // send complete
	void disconnect();
	void packetHandling(CPacket* packet);
	void heartbeat_check();
	void interserver_connect(char* ip, int port);
	void interserver_listen(int port);

	void recieve(char* buf, int size);
	void _send(char* buf, int size);
	void _sendHandling();

	void start(int type, int port);

private:
	std::queue<WSABUF *> sending_queue;
	MemPooler<WSABUF> * WSABUFPoolManager;

	CIocpHandler IocpHandler;
	PerSocketContext SocketContext;

	CRITICAL_SECTION disconnection_lock;
	CRITICAL_SECTION sending_queue_lock;

};