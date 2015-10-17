#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

extern bool g_bExitSignal;

int t_cnt = 0;

void CClient::Update(long long elapsed)
{
	CConnection *tConnection;

	if (NULL != connections[0])
	{
		connections[0]->GetStateMachine()->Update(elapsed);

		tConnection = connections[0];
	}

	if (NULL != connections[1])
	{
		connections[1]->GetStateMachine()->Update(elapsed);

		tConnection = connections[1];
	}

	// Both server are connected when this client is in escaping.
	if (2 == t_cnt)
	{
		std::cout << "[Escaping] The both server are being connected." << std::endl;
	}
	else if (1 == t_cnt)
	{
		bool chat = false;

		State<CConnection> *tState = tConnection->GetStateMachine()->CurrentState();

		if (CLobby::Instance() == tState || CRoom::Instance() == tState)
		{
			int this_room_num = tConnection->m_nRoom_num;

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
					tConnection->buf_str += '\0';
					if (tConnection->buf_str.size() < 1)
					{
						std::cout << "input process error !!!" << std::endl;
					}
					else if (tConnection->buf_str.size() > 4)
					{
						std::string subedstr = tConnection->buf_str.substr(0, 4);

						if (CLobby::Instance() == tState)
						{
							if (subedstr == "crea")
							{
								int t_room_num;

								int sscanf_result = sscanf_s(tConnection->buf_str.c_str(), "create %d\n", &t_room_num);

								if (sscanf_result != 1)
								{
									std::cout << "The 'create' command has been used incorrectly !" << std::endl;
								}

								tConnection->m_nTmpRoom_num = t_room_num;

								tConnection->SendCreateMessage(t_room_num);
							}
							else if (subedstr == "dest")
							{
								int t_room_num;
								int sscanf_result = sscanf_s(tConnection->buf_str.c_str(), "destroy %d\n", &t_room_num);

								if (sscanf_result != 1)
								{
									fputs("The 'destroy' command has been used incorrectly !\n", stdout);
								}

								tConnection->m_nTmpRoom_num = t_room_num;

								tConnection->SendDestroyMessage(t_room_num);
							}
							else if (subedstr == "join")
							{
								if (this_room_num == -1)
								{
									int t_room_num;
									char t_nick[20];
									int sscanf_result = sscanf(tConnection->buf_str.c_str(), "join %d %s\n", &t_room_num, t_nick);

									if (sscanf_result != 2)
									{
										std::cout << "The 'join' command has been used incorrectly !" << std::endl;
									}

									tConnection->m_nTmpRoom_num = t_room_num;
									strcpy_s(tConnection->m_vcNickname, t_nick);

									tConnection->SendJoinMessage(t_room_num, t_nick);
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
						else if (CRoom::Instance() == tState)
						{
							if (subedstr == "leav")
							{
								if (this_room_num == -1)
								{
									std::cout << "You must try to leave a room after you entered the room." << std::endl;
								}
								else
								{
									int sscanf_result = sscanf_s(tConnection->buf_str.c_str(), "leave\n");

									if (sscanf_result != 0)
									{
										std::cout << "The 'leave' command has been used incorrectly !" << std::endl;
									}

									tConnection->SendLeaveMessage();
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

							if (chat)
							{
								tConnection->SendChatMessage(tConnection->buf_str.c_str());
							}
						}
					}
					else
					{
						std::cout << "You have inputed invalid command." << std::endl;
					}

					tConnection->buf_str = "";

					//---------------------------------------------------------------

					/*while (tConnection->GetStateMachine()->CurrentState() != CLobby::Instance()
					&& tConnection->GetStateMachine()->CurrentState() != CRoom::Instance())
					{
					Sleep(100);
					}

					tConnection->printPrompt();*/
				}
				else
					tConnection->buf_str += inputed;
			}
		}
	}
	else if (0 == t_cnt)
	{
		exit(1);
	}
}

CClient::CClient()
{
	connections[0] = NULL;
	connections[1] = NULL;

	// local variables
	int port;
	unsigned long ip;
	std::string buf_str, wanted_str;

	while (1)
	{
		std::cout << "[input the ip address to be connected] ";
		std::getline(std::cin, buf_str);

		if (buf_str.size() > 0)
			ip = inet_addr(buf_str.substr(0, buf_str.size()).c_str());
		else
		{
			std::cout << "Error : The size of the input buffer is a zero or a negative number." << std::endl;
		}
		
		if (ip != INADDR_NONE)
		{
			break;
		}
		else
		{
			std::cout << "Error : This is not a legitimate Internet address." << std::endl;
		}
	}

	while (1)
	{
		std::cout << "[input the port number of the server] ";
		std::getline(std::cin, buf_str);

		if (buf_str.size() > 0)
			port = stoi(buf_str);
		else
		{
			std::cout << "Error : The size of the input buffer is a zero or a negative number." << std::endl;
		}
		

		if (port < 1025 || port > 65535)
		{
			std::cout << "The rang of port number must be in 1025 ~ 65535" << std::endl;
		}
		else
		{
			break;
		}
	}

	connections[0] = new CConnection(ip, port);
}

CClient::~CClient()
{
}
