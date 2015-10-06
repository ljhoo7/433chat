#include "stdafx.h"

InterSocket::InterSocket(TcpInterServer* InterServer){
	InterServer_ = InterServer;
}


void InterSocket::RecvProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->RecvProcess(isError, act, bytes_transferred);
}

void InterSocket::SendProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->SendProcess(isError, act, bytes_transferred);
}

void InterSocket::AcceptProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->AcceptProcess(isError, act, bytes_transferred);
}

void InterSocket::DisconnProcess(bool isError, Act* act, DWORD bytes_transferred){
	this->InterServer_->AcceptProcess(isError, act, bytes_transferred);
}

void InterSocket::ConnProcess(bool isError, Act* act, DWORD bytes_transferred){
	return;
}
