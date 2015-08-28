/////////////////////////////////////////
// Server.cpp
// 다음은 서버 프로그램입니다.
// 물론 Win32 프로그램으로 작성할 땐 Thread를 이용해 빠져야 할 부분입니다.
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
void Reverse(REV_STRUCT *st) {
	int size = ntohl(st->size) - 4;
	for (int i = 0; i < size / 2; ++i) {
		char temp = st->p[i];
		st->p[i] = st->p[size - 2 - i];
		st->p[size - 2 - i] = temp;
	}
}
DWORD WINAPI ReverseString(void* p) {
	SOCKET s = (SOCKET)p;
	REV_STRUCT pkt = { 0 };

	int current = 0;
	int n = recv(s, (char*)&pkt, 4, 0);
	int total = ntohl(pkt.size);

	current += n;
	while (total > current) {
		n = recv(s, (char*)&pkt + current, total - current, 0);
		if (n > 0) {
			current += n;
			continue;
		}
		else if (n <= 0) {
			cout << "Error 발생 : " << WSAGetLastError() << endl;
			return -1;
		}
	} // while
	Reverse(&pkt);
	send(s, (char*)&pkt, total, 0);
	return 0;
}
void main() {
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		cout << "winsock 초기화 실패 : " << WSAGetLastError() << endl;
		return;
	} // if
	//----------------------------------------------------------
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == SOCKET_ERROR) {
		cout << "소켓이 생성되지 않았습니다." << endl;
		return;
	}
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4500);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(s, (SOCKADDR*)&addr, sizeof(addr)) == -1) {
		cout << "Failed binding" << endl;
		return;
	}
	if (listen(s, 5) == -1) {
		cout << "Failed listening" << endl;
		return;
	}
	cout << "Waiting for connencting from Client..." << endl;
	while (1) {
		int size = sizeof(addr);
		SOCKET c_s = accept(s, (SOCKADDR*)&addr, &size);
		cout << "접속된 Client : " << inet_ntoa(addr.sin_addr) << endl;
		CloseHandle(CreateThread(0, 0, ReverseString, (void*)c_s, 0, 0));
	} // while( 1 )
	closesocket(s);
	//----------------------------------------------------------
	WSACleanup();
}
//////////////////////////////////////////////////////////////////