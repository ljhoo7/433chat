#include "stdafx.h"
#include "ServerAgent.h"


ServerAgent::ServerAgent(WORD port, int ThreadPoolSize, int SocketPoolSize)
:WinSockBase(),
 mThreadPoolSize(ThreadPoolSize),
 mSocketPoolSize(SocketPoolSize),
 mPort(port)
{
	
	mProactor		 = nullptr;
	mAcceptor		 = nullptr;
	mDisconnector    = nullptr;
	mReceiver		 = nullptr;
	mSender			 = nullptr;

	mListenSocket	 = nullptr;

}

ServerAgent::~ServerAgent()
{
}

void ServerAgent::Run()
{
	mProactor		 = new Proactor(mThreadPoolSize);
	mDisconnector	 = new Disconnector(mProactor);
	mAcceptor		 = new Acceptor;
	mReceiver		 = new CReceiver(mProactor);
	mSender			 = new CSender(mProactor);

	mListenSocket	 = new TcpListenSocket;

	mListenSocket->Init(mPort);
	mListenSocket->Listen(mProactor);

	mAcceptor->Init(mListenSocket, mProactor);

	// Create Socket Pool

	for (int i = 0; i < mSocketPoolSize; ++i)
	{
		SASocket* pSocket = new SASocket;
		pSocket->Init();
		pSocket->InitAct(mProactor,
						 mAcceptor,
						 mDisconnector,
						 NULL,
						 mSender,
						 mReceiver);

		mAcceptor->Register(*pSocket, 4);

	}
}


