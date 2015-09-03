#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

extern bool g_bExitSignal;

CLobby* CLobby::Instance()
{
	static CLobby instance;

	return &instance;
}

void CLobby::Enter(CClient *client)
{
}

void CLobby::Exit(CClient *client)
{
}

void CLobby::Execute(CClient *client)
{
	std::string buf_str;
	bool chat = false;
	int retval;

	int this_room_num = client->GetRoomNumber();

	// Input command
	if (this_room_num == -1)
		printf("[Not Connected] ");

	std::getline(std::cin, buf_str);

	if (buf_str.size() < 1)
	{

	}
	else if (buf_str.size() > 4)
	{
		std::string subedstr = buf_str.substr(0, 4);
		if (subedstr == "crea")
		{
			int t_room_num;

			int sscanf_result = sscanf(buf_str.c_str(), "create %d\n", &t_room_num);

			if (sscanf_result != 1)
			{
				std::cout << "The 'create' command has been used incorrectly !" << std::endl;
			}

			client->SendCreateMessage(t_room_num);
		}
		else if (subedstr == "dest")
		{
			int t_room_num;
			int sscanf_result = sscanf(buf_str.c_str(), "destroy %d\n", &t_room_num);

			if (sscanf_result != 1)
			{
				fputs("The 'destroy' command has been used incorrectly !\n", stdout);
			}

			client->SendDestroyMessage(t_room_num);
		}
		else if (subedstr == "join")
		{
			if (this_room_num == -1)
			{
				int t_room_num;
				char t_nick[20];
				int sscanf_result = sscanf(buf_str.c_str(), "join %d %s\n", &t_room_num, t_nick);

				if (sscanf_result != 2)
				{
					std::cout << "The 'join' command has been used incorrectly !" << std::endl;
				}

				client->SendJoinMessage(t_room_num, t_nick);
			}
			else
			{
				std::cout << "You must try to enter a room after you leaved your room." << std::endl;
			}
		}
		else if (subedstr == "exit")
		{
			g_bExitSignal = true;
		}
		else
		{
		}
	}
	else
	{
		std::cout << "You have inputed invalid command." << std::endl;
	}
}

//---------------------------------------------

CRoom* CRoom::Instance()
{
	static CRoom instance;

	return &instance;
}

void CRoom::Enter(CClient *client)
{
}

void CRoom::Exit(CClient *client)
{
}

void CRoom::Execute(CClient *client)
{
	std::string buf_str;
	bool chat = false;
	int retval;

	int this_room_num = client->GetRoomNumber();

	// Input command
	if (this_room_num == -1)
		printf("[Not Connected] ");

	std::getline(std::cin, buf_str);

	if (buf_str.size() < 1)
	{

	}
	else if (buf_str.size() > 4)
	{
		std::string subedstr = buf_str.substr(0, 4);
		if (subedstr == "leav")
		{
			if (this_room_num == -1)
			{
				std::cout << "You must try to leave a room after you entered the room." << std::endl;
			}
			else
			{
				int sscanf_result = sscanf(buf_str.c_str(), "leave\n");

				if (sscanf_result != 0)
				{
					std::cout << "The 'leave' command has been used incorrectly !" << std::endl;
				}

				client->SendLeaveMessage();
			}
		}
		else if (subedstr == "exit")
		{
			g_bExitSignal = true;
		}
		else
		{
			if (this_room_num > -1)
			{
				chat = true;
			}
			else
			{
				std::cout << "You must try to chat after you entered the room." << std::endl;
			}

		}
	}
	else
	{
		if (this_room_num > -1)
		{
			chat = true;
		}
		else
		{
			std::cout << "You must try to chat after you entered the room." << std::endl;
		}
	}

	if (chat)
	{
		client->SendChatMessage(buf_str.c_str());
	}
}

//---------------------------------------------

CDestroy_Response_Wait* CDestroy_Response_Wait::Instance()
{
	static CDestroy_Response_Wait instance;

	return &instance;
}

void CDestroy_Response_Wait::Enter(CClient *client)
{
}

void CDestroy_Response_Wait::Exit(CClient *client)
{
}

void CDestroy_Response_Wait::Execute(CClient *client)
{
}

//---------------------------------------------

CJoin_Response_Wait* CJoin_Response_Wait::Instance()
{
	static CJoin_Response_Wait instance;

	return &instance;
}

void CJoin_Response_Wait::Enter(CClient *client)
{
}

void CJoin_Response_Wait::Exit(CClient *client)
{
}

void CJoin_Response_Wait::Execute(CClient *client)
{
}

//---------------------------------------------

CLeave_Response_Wait* CLeave_Response_Wait::Instance()
{
	static CLeave_Response_Wait instance;

	return &instance;
}

void CLeave_Response_Wait::Enter(CClient *client)
{
}

void CLeave_Response_Wait::Exit(CClient *client)
{
}

void CLeave_Response_Wait::Execute(CClient *client)
{
}

//---------------------------------------------

CCreate_Response_Wait* CCreate_Response_Wait::Instance()
{
	static CCreate_Response_Wait instance;

	return &instance;
}

void CCreate_Response_Wait::Enter(CClient *client)
{
}

void CCreate_Response_Wait::Exit(CClient *client)
{
}

void CCreate_Response_Wait::Execute(CClient *client)
{
	
}