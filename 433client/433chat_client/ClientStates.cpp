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
	std::cout << "Entered Lobby State" << std::endl;
}

void CLobby::Exit(CClient *client)
{
}

void CLobby::Execute(CClient *client, long long elapsed)
{
	bool chat = false;

	int this_room_num = client->GetRoomNumber();

	int c = std::cin.peek();
	if (c == EOF)
	{
		return;
	}
	else
	{
		char inputed;
		std::cin.get(inputed);

		if (inputed == '\n')
		{
			if (client->buf_str.size() < 1)
			{
				std::cout << "input process error !!!" << std::endl;
			}
			else if (client->buf_str.size() > 4)
			{
				std::string subedstr = client->buf_str.substr(0, 4);
				if (subedstr == "crea")
				{
					int t_room_num;

					int sscanf_result = sscanf_s(client->buf_str.c_str(), "create %d\n", &t_room_num);

					if (sscanf_result != 1)
					{
						std::cout << "The 'create' command has been used incorrectly !" << std::endl;
					}

					client->m_nTmpRoom_num = t_room_num;

					client->SendCreateMessage(t_room_num);
				}
				else if (subedstr == "dest")
				{
					int t_room_num;
					int sscanf_result = sscanf_s(client->buf_str.c_str(), "destroy %d\n", &t_room_num);

					if (sscanf_result != 1)
					{
						fputs("The 'destroy' command has been used incorrectly !\n", stdout);
					}

					client->m_nTmpRoom_num = t_room_num;

					client->SendDestroyMessage(t_room_num);
				}
				else if (subedstr == "join")
				{
					if (this_room_num == -1)
					{
						int t_room_num;
						char t_nick[20];
						int sscanf_result = sscanf_s(client->buf_str.c_str(), "join %d %s\n", &t_room_num, t_nick);

						if (sscanf_result != 2)
						{
							std::cout << "The 'join' command has been used incorrectly !" << std::endl;
						}

						client->m_nTmpRoom_num = t_room_num;
						strcpy_s(client->m_vcNickname, t_nick);

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
					std::cout << "You have inputed invalid command." << std::endl;
				}
			}
			else
			{
				std::cout << "You have inputed invalid command." << std::endl;
			}

			client->buf_str = "";

			//---------------------------------------------------------------
			
			/*while (client->GetStateMachine()->CurrentState() != CLobby::Instance()
				&& client->GetStateMachine()->CurrentState() != CRoom::Instance())
			{
				Sleep(100);
			}

			client->printPrompt();*/
		}
		else
			client->buf_str += inputed;
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
	std::cout << "Entered Room State" << std::endl;
}

void CRoom::Exit(CClient *client)
{
}

void CRoom::Execute(CClient *client, long long elapsed)
{
	bool chat = false;

	int this_room_num = client->GetRoomNumber();

	int c = std::cin.peek();
	if (c == EOF)
	{
		return;
	}
	else
	{
		char inputed;
		std::cin.get(inputed);

		if (inputed == '\n')
		{
			if (client->buf_str.size() < 1)
			{
				std::cout << "input process error !!!" << std::endl;
			}
			else if (client->buf_str.size() > 4)
			{
				std::string subedstr = client->buf_str.substr(0, 4);
				if (subedstr == "leav")
				{
					if (this_room_num == -1)
					{
						std::cout << "You must try to leave a room after you entered the room." << std::endl;
					}
					else
					{
						int sscanf_result = sscanf_s(client->buf_str.c_str(), "leave\n");

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
				client->SendChatMessage(client->buf_str.c_str());
			}

			client->buf_str = "";

			//---------------------------------------------------------------

			/*while (client->GetStateMachine()->CurrentState() != CLobby::Instance()
				&& client->GetStateMachine()->CurrentState() != CRoom::Instance())
			{
				Sleep(100);
			}

			client->printPrompt();*/
		}
		else
			client->buf_str += inputed;
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
	std::cout << "Entered Destroy Response State" << std::endl;
	client->accumTime = 0;
}

void CDestroy_Response_Wait::Exit(CClient *client)
{
}

void CDestroy_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->accumTime += elapsed;
	if (client->accumTime > ELAPSED_DEST)
	{
		client->SendDestroyMessage(client->m_nTmpRoom_num);

		client->accumTime = 0;
	}
}

//---------------------------------------------

CJoin_Response_Wait* CJoin_Response_Wait::Instance()
{
	static CJoin_Response_Wait instance;

	return &instance;
}

void CJoin_Response_Wait::Enter(CClient *client)
{
	std::cout << "Entered Join Response State" << std::endl;
	client->accumTime = 0;
}

void CJoin_Response_Wait::Exit(CClient *client)
{
}

void CJoin_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->accumTime += elapsed;
	if (client->accumTime > ELAPSED_DEST)
	{
		client->SendJoinMessage(client->m_nTmpRoom_num, client->m_vcNickname);

		client->accumTime = 0;
	}
}

//---------------------------------------------

CLeave_Response_Wait* CLeave_Response_Wait::Instance()
{
	static CLeave_Response_Wait instance;

	return &instance;
}

void CLeave_Response_Wait::Enter(CClient *client)
{
	std::cout << "Entered Leave Response State" << std::endl;
	client->accumTime = 0;
}

void CLeave_Response_Wait::Exit(CClient *client)
{
}

void CLeave_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->accumTime += elapsed;
	if (client->accumTime > ELAPSED_DEST)
	{
		client->SendLeaveMessage();

		client->accumTime = 0;
	}
}

//---------------------------------------------

CCreate_Response_Wait* CCreate_Response_Wait::Instance()
{
	static CCreate_Response_Wait instance;

	return &instance;
}

void CCreate_Response_Wait::Enter(CClient *client)
{
	std::cout << "Entered Create Response State" << std::endl;
	client->accumTime = 0;
}

void CCreate_Response_Wait::Exit(CClient *client)
{
}

void CCreate_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->accumTime += elapsed;
	if (client->accumTime > ELAPSED_DEST)
	{
		client->SendCreateMessage(client->m_nTmpRoom_num);

		client->accumTime = 0;
	}
}