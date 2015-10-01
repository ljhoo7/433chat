#include "stdafx.h"

CBufPoolManager::CBufPoolManager(int count){
	for (int i = 0; i < count; i++){
		bufs.push_back(new char[BUFSIZE]);
	}
}

CBufPoolManager::~CBufPoolManager(){
	for (unsigned int i = 0; i < bufs.size(); i++){
		delete bufs[i];
	}
}

char* CBufPoolManager::pop(){
	if (bufs.size() > 0){
		char* a = bufs[bufs.size() - 1];
		bufs.pop_back();
		return a;
	}
	else{
		return new char[BUFSIZE];
	}
}

void CBufPoolManager::push(char* a){
	bufs.push_back(a);
}
