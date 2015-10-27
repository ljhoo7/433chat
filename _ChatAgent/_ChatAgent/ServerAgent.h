#pragma once

#include "Common.h"


class ServerAgent : public WinSockBase
{

public:
	ServerAgent(WORD port, int ThreadPoolSize, int SocketPoolSize = 1);
	virtual ~ServerAgent();

public:
	void Run();

private:
	Proactor*						mProactor;
	Acceptor*						mAcceptor;
	Disconnector*					mDisconnector;
	Receiver*						mReceiver;
	Sender*							mSender;

	TcpListenSocket*				mListenSocket;

	int								mThreadPoolSize;
	int								mSocketPoolSize;


	WORD							mPort;
	
};

