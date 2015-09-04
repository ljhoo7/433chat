#include "bufPoolManager.h"
#include "packet.h"

BufPoolManager::BufPoolManager(int count){
	for (int i = 0; i < count; i++){
		bufs.push_back(new char[BUFSIZE]);
	}
}

BufPoolManager::~BufPoolManager(){
	for (unsigned int i = 0; i < bufs.size(); i++){
		delete bufs[i];
	}
}

char* BufPoolManager::pop(){
	if (bufs.size() > 0){
		char* a = bufs[bufs.size() - 1];
		bufs.pop_back();
		return a;
	}
	else{
		return new char[BUFSIZE];
	}
}

void BufPoolManager::push(char* a){
	bufs.push_back(a);
}


PacketPoolManager::PacketPoolManager(int count){
	for (int i = 0; i < count; i++){
		bufs.push_back(new Packet());
	}
}

PacketPoolManager::~PacketPoolManager(){
	for (unsigned int i = 0; i < bufs.size(); i++){
		delete bufs[i];
	}
}

Packet* PacketPoolManager::pop(){
	if (bufs.size() > 0){
		Packet* a = bufs[bufs.size() - 1];
		bufs.pop_back();
		return a;
	}
	else{
		return new Packet();
	}
}

void PacketPoolManager::push(Packet* a){
	bufs.push_back(a);
}