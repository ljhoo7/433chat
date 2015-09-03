#include "Headers.h"
#include "..\Utilities.h"

CNotConnected* CNotConnected::Instance()
{
	static CNotConnected instance;

	return &instance;
}

void CNotConnected::Enter(CClient& client)
{
}

void CNotConnected::Exit(CClient& client)
{
}

void CNotConnected::Execute(CClient& client)
{
	// Input command
	if (client.room_num == -1)
		printf("[Not Connected] ");
	if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
		break;

	bool chat = false;
	std::string tmpstr = buf;
	if (tmpstr.size() > 4)
	{
		std::string subedstr = tmpstr.substr(0, 4);
		if (subedstr == "crea")
		{
			int t_room_num;

			int sscanf_result = sscanf(tmpstr.c_str(), "create %d\n", &t_room_num);

			if (sscanf_result != 1)
			{
				fputs("The 'create' command has been used incorrectly !\n", stdout);
			}

			if (t_room_num < 0)
			{
				fputs("���ȣ�� 0 �̻��̾�� �˴ϴ�.\n", stdout);
				continue;
			}

			// �� ���� ������
			t_create tmp_packet;
			int size = sizeof(t_create);
			tmp_packet.length = size;
			tmp_packet.type = pkt_type::pt_create;
			//std::chrono::steady_clock::duration tmptime = std::chrono::steady_clock::now().time_since_epoch();
			//tmp_packet.time_stamp = tmptime.count();
			//std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::to_tmp_packet.time_stamp);
			//std::cout << tmp_packet.time_stamp << std::endl;
			tmp_packet.room_num = t_room_num;

			// �� ���� ������ ������
			retval = send(sock, (char*)&tmp_packet, size, 0);
			if (retval == SOCKET_ERROR){
				err_display("send()");
				return 0;
			}
			printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
		}
		else if (subedstr == "join")
		{
			if (room_num == -1)
			{
				int sscanf_result = sscanf(tmpstr.c_str(), "join %d %s\n", &room_num, buf2);

				if (sscanf_result != 2)
				{
					fputs("The 'join' command has been used incorrectly !\n", stdout);
				}

				if (room_num < 0)
				{
					fputs("���ȣ�� 0 �̻��̾�� �˴ϴ�.\n", stdout);
					continue;
				}

				// '\n' ���� ����
				len = strlen(buf2);

				strcpy(nickname, buf2);

				if (buf2[len - 1] == '\n')
					buf2[len - 1] = '\0';
				if (strlen(buf2) == 0)
					break;

				// �Խ� ������
				t_join tmp_packet;
				int size = len + sizeof(int)+sizeof(short)+sizeof(short);
				tmp_packet.length = size;
				tmp_packet.type = pkt_type::pt_join;
				tmp_packet.room_num = room_num;
				strcpy(tmp_packet.nickname, buf2);

				// �Խ� ������ ������
				retval = send(sock, (char*)&tmp_packet, size, 0);
				if (retval == SOCKET_ERROR){
					err_display("send()");
					break;
				}

				printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
			}
			else
			{
				fputs("��� �� �Խ��� �õ��ϼ���.\n", stdout);
			}
		}
		else if (subedstr == "leav")
		{
			if (room_num == -1)
			{
				fputs("�Խ� �� ����� �õ��ϼ���.\n", stdout);
			}
			else
			{
				int sscanf_result = sscanf(tmpstr.c_str(), "leave\n");

				if (sscanf_result != 0)
				{
					fputs("The 'leave' command has been used incorrectly !\n", stdout);
				}

				if (room_num < 0)
				{
					fputs("���ȣ�� 0 �̻��̾�� �˴ϴ�.\n", stdout);
					continue;
				}

				strcpy(buf2, nickname);

				// '\n' ���� ����
				len = strlen(buf2);

				strcpy(nickname, "");

				if (buf2[len - 1] == '\n')
					buf2[len - 1] = '\0';
				if (strlen(buf2) == 0)
					break;

				// ��� ������
				t_leave tmp_packet;
				int size = len + sizeof(short)+sizeof(short)+sizeof(int);
				tmp_packet.length = size;
				tmp_packet.type = pkt_type::pt_leave;
				tmp_packet.room_num = room_num;
				strcpy(tmp_packet.nickname, buf2);

				// ��� ������ ������
				retval = send(sock, (char*)&tmp_packet, size, 0);
				if (retval == SOCKET_ERROR){
					err_display("send()");
					break;
				}

				printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

				room_num = -1;
			}
		}
		else if (subedstr == "dest")
		{
			int t_room_num;
			int sscanf_result = sscanf(tmpstr.c_str(), "destroy %d\n", &t_room_num);

			if (sscanf_result != 1)
			{
				fputs("The 'destroy' command has been used incorrectly !\n", stdout);
			}

			if (t_room_num < 0)
			{
				fputs("���ȣ�� 0 �̻��̾�� �˴ϴ�.\n", stdout);
				continue;
			}

			// �� ���� ������
			t_destroy tmp_packet;
			int size = sizeof(t_destroy);
			tmp_packet.length = size;
			tmp_packet.type = pkt_type::pt_destroy;
			tmp_packet.room_num = t_room_num;

			// �� ���� ������ ������
			retval = send(sock, (char*)&tmp_packet, size, 0);
			if (retval == SOCKET_ERROR){
				err_display("send()");
				return 0;
			}
			printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
		}
		else
		{
			if (room_num > -1)
			{
				chat = true;
			}
			else
			{
				fputs("�Խ� �� ä���� �õ��ϼ���.\n", stdout);
			}

		}
	}
	else
	{
		if (room_num > -1)
		{
			chat = true;
		}
		else
		{
			fputs("�Խ� �� ä���� �õ��ϼ���.\n", stdout);
		}
	}

	if (chat)
	{
		// '\n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		t_chat tmp_packet;
		int size = len + 20 + sizeof(int)+sizeof(short)+sizeof(short);
		tmp_packet.length = size;
		tmp_packet.type = pkt_type::pt_chat;
		tmp_packet.room_num = room_num;
		strcpy(tmp_packet.str, buf);
		strcpy(tmp_packet.nickname, nickname);

		// ä�� ������ ������
		retval = send(sock, (char*)&tmp_packet, size, 0);
		if (retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		//	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
	}
}

//---------------------------------------------

CLobby* CLobby::Instance()
{
	static CLobby instance;

	return &instance;
}

void CLobby::Enter(CClient& client)
{
}

void CLobby::Exit(CClient& client)
{
}

void CLobby::Execute(CClient& client)
{
}

//---------------------------------------------

CRoom* CRoom::Instance()
{
	static CRoom instance;

	return &instance;
}

void CRoom::Enter(CClient& client)
{
}

void CRoom::Exit(CClient& client)
{
}

void CRoom::Execute(CClient& client)
{
}