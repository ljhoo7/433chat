#include "stdafx.h"

CPacketPoolManager::CPacketPoolManager(int count){
	for (int i = 0; i < count; i++){
		bufs.push_back(new CPacket());
	}
}

CPacketPoolManager::~CPacketPoolManager(){
	for (unsigned int i = 0; i < bufs.size(); i++){
		delete bufs[i];
	}
}

CPacket* CPacketPoolManager::pop(){
	if (bufs.size() > 0){
		CPacket* a = bufs[bufs.size() - 1];
		bufs.pop_back();
		return a;
	}
	else{
		return new CPacket();
	}
}

void CPacketPoolManager::push(CPacket* a){
	bufs.push_back(a);
}
