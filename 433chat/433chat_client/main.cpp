//////////////////////////////////////////////////////////////////
// 다음은 Client.cpp 입니다.
#define WIN32_LEAN_AND_MEAN
// windows.h 파일에서 자주 사용하지 않은 요소를 컴파일에서 제외시킨다.
// 컴파일 속도가 빨라지고 winsock2.h와의 충돌이 일어나지 않는다.
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include "..\skel.h"
using namespace std;
#pragma comment ( lib, "ws2_32.lib" )
#pragma pack( 1 ) // 구조체에서 1byte 단위로 메모리를 Align하라는 지시어
void main(int argc, char **argv) {
	if (argc != 3) {
		cout << "ReverseClient <ip> <message>" << endl;
		return;
	}
	char *ip = argv[1];
	char *str = argv[2];
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		cout << "winsock 초기화 실패 : " << WSAGetLastError() << endl;
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
	cout << "전송된 문자열 : " << pkt.p << endl;
	closesocket(s);
	//----------------------------------------------------------
	WSACleanup();
}
/////////////////////////////////////////////////////////////////////////