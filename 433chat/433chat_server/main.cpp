/////////////////////////////////////////
// Server.cpp
// ������ ���� ���α׷��Դϴ�.
// ���� Win32 ���α׷����� �ۼ��� �� Thread�� �̿��� ������ �� �κ��Դϴ�.
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
			cout << "Error �߻� : " << WSAGetLastError() << endl;
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
		cout << "winsock �ʱ�ȭ ���� : " << WSAGetLastError() << endl;
		return;
	} // if
	//----------------------------------------------------------
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == SOCKET_ERROR) {
		cout << "������ �������� �ʾҽ��ϴ�." << endl;
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
		cout << "���ӵ� Client : " << inet_ntoa(addr.sin_addr) << endl;
		CloseHandle(CreateThread(0, 0, ReverseString, (void*)c_s, 0, 0));
	} // while( 1 )
	closesocket(s);
	//----------------------------------------------------------
	WSACleanup();
}
//////////////////////////////////////////////////////////////////