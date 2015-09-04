#include "Packet.h"

Packet::Packet(){
	type = 0;
	owner = NULL;
	msg = NULL;
}

Packet::Packet(UserToken* owner, char* msg, int type){
	this->owner = owner;
	this->msg = msg;
	this->type = type;
}