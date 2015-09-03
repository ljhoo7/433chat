#include "interServer.h"
#include "logic.h"
#include "packet.h"

extern LogicHandle logicHandle;

InterServer::InterServer() : poolManager(10), packetPoolManager(10), 
process_thread(), heart_thread(), listen_thread(){
	the_other_sock = NULL;
}

void InterServer::start(int type, int port){
	this->type = type;

	if (type == 0){
		interserver_connect(CONNECTIP, port);
	}
	else{
		interserver_listen(port);
	}


}

void InterServer::interserver_connect(char* ip, int port){	
	printf("This server is attempting to connect...\n");
	the_other_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (the_other_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(port);

	int retval = connect(the_other_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	printf("This server has been connected to the other server by connect().\n");

	printf("InterServer Thread has been activated.\n");

	process_thread = std::thread(&InterServer::process, this);
	process_thread.join();

}

void InterServer::interserver_listen(int port){
	printf("This server will be waiting for The other server.\n");

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
	int retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	listen_thread = std::thread(&InterServer::listenProcess, this);
}

void InterServer::listenProcess(){
	SOCKADDR_IN theotheraddr;
	int addrlen;

	while (true){
		addrlen = sizeof(theotheraddr);
		the_other_sock = accept(listen_sock, (SOCKADDR*)&theotheraddr, &addrlen);
		if (the_other_sock == INVALID_SOCKET)
		{
			err_quit("accept()");
		}
		printf("This server has been connected to the other server by accept().\n");
		printf("InterServer Thread has been activated.\n");

		process_thread = std::thread(&InterServer::process, this);
		heart_thread = std::thread(&InterServer::heartbeat_check, this);

		process_thread.join();
		heart_thread.join();
	}
	
}
void InterServer::process(){
	int fps = 30;
	double block = 1000 / fps;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;
	long long time;

	start_time = std::chrono::system_clock::now();

	while (1)
	{
		if (the_other_sock == NULL) return;
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time){
			recieveProcess();
			start_time = std::chrono::system_clock::now();
		}
		
	}
}

/* 
	any thread
	need to handle userlist lock
*/
void InterServer::disconnect(){
	if (the_other_sock != NULL){
		printf("closed the other server.\n");
		closesocket(the_other_sock);
		the_other_sock = NULL;
	}
}

/* interServer recieve thread */
void InterServer::recieveProcess(){
	if (the_other_sock == NULL) return;
	char* buf = poolManager.pop();
	recieve(buf, sizeof(short));
	/* packet handling */

	ssType _type = (ssType)((unsigned short)(*buf));
	
	switch (_type){
		
		case ssType::pkt_create:
			recieve(buf + 2, sizeof(ss_create)-2);
			break;
		case ssType::pkt_destroy:
			recieve(buf + 2, sizeof(ss_destroy)-2);
			break;
		case ssType::pkt_join:
			recieve(buf + 2, sizeof(ss_join)-2);
			break;
		case ssType::pkt_leave:
			recieve(buf + 2, sizeof(ss_leave)-2);
			break;
		case ssType::pkt_connect:
			recieve(buf + 2, sizeof(ss_connect)-2);
			break;
		case ssType::pkt_disconnect:
			recieve(buf + 2, sizeof(ss_disconnect)-2);
			break;
		case ssType::pkt_heartbeats:
			recieve(buf + 2, sizeof(ss_heartbeats)-2);
			break;
		case ssType::pkt_heartbeats_response:
			recieve(buf + 2, sizeof(ss_heartbeats_response)-2);
			break;
		case ssType::pkt_room_info_success:
			recieve(buf + 2, sizeof(ss_room_info_success)-2);
			break;
		case ssType::pkt_player_info_success:
			recieve(buf + 2, sizeof(ss_player_info_success)-2);
			break;
		case ssType::pkt_room_info_failure:
			recieve(buf + 2, sizeof(ss_room_info_failure)-2);
			break;
		case ssType::pkt_player_info_failure:
			recieve(buf + 2, sizeof(ss_player_info_failure)-2);
			break;
			
			/* var data */
		case ssType::pkt_chat:
		case ssType::pkt_room_info_send:
		case ssType::pkt_player_info_send:
			recieve(buf + 2, sizeof(short));
			unsigned short size;
			memcpy(&size, buf + 2, sizeof(short));
			recieve(buf + 4, size);
			break;
		default:
			disconnect();
			break;
	}

	Packet* msg = packetPoolManager.pop();
	msg->type = this->type;
	msg->owner = NULL;
	msg->msg = buf;

	logicHandle.enqueue_oper(msg);
}
void InterServer::recieve(char* buf, int size){
	if (size == 0) return;
	int ret = recv(the_other_sock, buf, size, 0);
	if (ret == SOCKET_ERROR){
		printf("recieve error\n");
		disconnect();
	}
}

void InterServer::_send(char* buf, int size){
	int ret = send(the_other_sock, buf, size, 0);
	if (ret == SOCKET_ERROR){
		printf("send error\n");
		disconnect();
	}
}

void InterServer::heartbeat_check() {
	while (true){
		beat_check = false;
		if (the_other_sock == NULL) break;

		printf("hearth check send!\n");
		ss_heartbeats msg;
		msg.type = ssType::pkt_heartbeats;
		_send((char *)&msg, sizeof(msg));
		std::this_thread::sleep_for(std::chrono::seconds(5));
		if (beat_check == false) break;
	}
	printf("connection fail\n");
	disconnect();
}

/* logic thread */
void InterServer::packetHandling(Packet* packet){
	if (the_other_sock == NULL){
		return;
	}
	
	ssType _type = (ssType)(*packet->msg);

	switch (_type){
	case ssType::pkt_heartbeats:
		printf("recieve heartbeat check. send response\n");
		ss_heartbeats_response msg;
		msg.type = ssType::pkt_heartbeats_response;
		_send((char *)&msg, sizeof(msg));
		break;
	case ssType::pkt_heartbeats_response:
		printf("recieve heartbeat response\n");
		beat_check = true;
		break;
	}
}