#include "Receiver.h"

void Receiver::ProcEvent(Act* act, DWORD bytes_transferred)
{
	assert(dynamic_cast<TcpAct*>(act));
	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);
	assert(tcpact.TcpSocket_);
	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	//printf("...Receiver (%d byte) s(%d)\n", bytes_transferred, tcpsocket.GetSocket() );

	// passive �������
	if (bytes_transferred == 0)
	{
		tcpsocket.Disconnect();
	}
	else
	{
		//printf("���� �� = %s\n", tcpsocket.RecvBuf_);

		// Async Recv ����Ʈ
		tcpsocket.Recv();

		//������ŭ ����
		tcpsocket.Send((BYTE*)tcpsocket.RecvBuf_, bytes_transferred);
	}

}

void Receiver::ProcError(Act* act, DWORD error)
{
	assert(dynamic_cast<TcpAct*>(act));

	TcpAct& tcpact = *dynamic_cast<TcpAct*>(act);

	assert(tcpact.TcpSocket_);

	TcpSocket& tcpsocket = *tcpact.TcpSocket_;

	//printf("...����ó�� Receiver s(%d) err(%d)\n", tcpsocket.GetSocket(), error );

	tcpsocket.Disconnect();

}

void Receiver::Init(Proactor* proactor)
{
	Proactor_ = proactor;
}