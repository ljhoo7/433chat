//////////////////////////////////////////////////////////////////
// ������ Client.cpp �Դϴ�.
#define WIN32_LEAN_AND_MEAN
// windows.h ���Ͽ��� ���� ������� ���� ��Ҹ� �����Ͽ��� ���ܽ�Ų��.
// ������ �ӵ��� �������� winsock2.h���� �浹�� �Ͼ�� �ʴ´�.
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include "..\skel.h"
using namespace std;
#pragma comment ( lib, "ws2_32.lib" )
#pragma pack( 1 ) // ����ü���� 1byte ������ �޸𸮸� Align�϶�� ���þ�
void main(int argc, char **argv) {
	if (argc != 3) {
		cout << "ReverseClient <ip> <message>" << endl;
		return;
	}
	char *ip = argv[1];
	char *str = argv[2];
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		cout << "winsock �ʱ�ȭ ���� : " << WSAGetLastError() << endl;
		return;
	} // if
	//----------------------------------------------------------
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == SOCKET_ERROR) {
		cout << "Socket Error!" << endl;
		return;
	}
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4500);
	addr.sin_addr.s_addr = inet_addr(ip);
	if (connect(s, (SOCKADDR*)&addr, sizeof(addr)) == -1) {
		cout << "Connent error!!" << endl;
		return;
	}
	cout << "Succeed connecting" << endl;
	REV_STRUCT pkt;
	pkt.size = strlen(str) + 5;
	int size = pkt.size;
	pkt.size = htonl(size);
	strcpy(pkt.p, argv[2]);
	int n = send(s, (char*)&pkt, size, 0);
	//int n = send( s, (char*)&msg, htonl( msg.size ), 0 );
	// int n2 = send( s, (char*)&msg, htonl( msg.size ), 0 );
	int n2 = recv(s, (char*)&pkt, size, 0);
	cout << "���۵� ���ڿ� : " << pkt.p << endl;
	closesocket(s);
	//----------------------------------------------------------
	WSACleanup();
}
/////////////////////////////////////////////////////////////////////////