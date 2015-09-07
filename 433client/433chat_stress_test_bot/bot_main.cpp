#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

bool				g_bExitSignal;

int					g_nTime_span;
int					g_nRoom_num;
int					g_nBot_num;
unsigned long		ip;
int					port;

int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		std::cout << "(program name) (server_ip) (server_port) (bot_num) (time_span)" << std::endl;
		return 0;
	}

	ip = inet_addr(argv[1]);

	if (ip == INADDR_NONE)
	{
		std::cout << "Error : This is not a legitimate Internet address." << std::endl;
		return 0;
	}

	port = atoi(argv[2]);

	if (port < 1025 || port > 65535)
	{
		std::cout << "The rang of port number must be in 1025 ~ 65535" << std::endl;
		return 0;
	}

	g_nTime_span = atoi(argv[4]);

	if (g_nTime_span < 1)
	{
		std::cout << "the time span be an integer number which is bigger than 0" << std::endl;
		return 0;
	}

	g_nBot_num = atoi(argv[3]);

	if (g_nBot_num < 1)
	{
		std::cout << "The number of Bots must be bigger than one." << std::endl;
		return 0;
	}

	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	int fps = 30;
	double block = 1000 / fps;
	long long time;
	long long prevTime = 0;

	int left, theRoom;

	g_bExitSignal = false;

	std::vector<CBot*> t_vClient;

	//CHECK_FAILURE(
		for (int m = 0; m < g_nBot_num; ++m)
		{
			CBot *tBot = new CBot(-1, g_nTime_span, 0);
			t_vClient.push_back(tBot);

			theRoom = (m / USER_MAX);
			left = (m % USER_MAX);
			if (!left)
			{
				tBot->m_nTmpRoom_num = theRoom;
				if (!tBot->SendCreateMessage(theRoom))
				{
					std::cout << "the " + std::to_string(theRoom) + "th room of bots are not created." << std::endl;
					return 0;
				}

				tBot->GetStateMachine()->ChangeState(CCreate_Response_Wait::Instance());
			}
			else
			{
				tBot->SendJoinMessage(theRoom, const_cast<char*>(("bot_" + std::to_string(m)).c_str()));
				
				tBot->GetStateMachine()->ChangeState(CJoin_Response_Wait::Instance());
			}
		}

		std::cout << "All Create&Join process has been done." << std::endl;

		// Keep Looping for each clients
		while (!g_bExitSignal){
			for (std::vector<CBot*>::iterator iter = t_vClient.begin()
				; iter != t_vClient.end(); ++iter)
			{
				tmp = std::chrono::system_clock::now() - start_time;
				tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
				time = tmp2.count();

				if (block <= time)
				{
					(*iter)->GetStateMachine()->Update(time);

					start_time = std::chrono::system_clock::now();
				}
			}
		}

		// Wait for multiple threads
		for (std::vector<CBot*>::iterator iter = t_vClient.begin()
			; iter != t_vClient.end(); ++iter)
		{
			(*iter)->GetReceivingThread()->join();
		}

		std::cout << "All thread has been joined." << std::endl;

		// Deallocating all
		for (std::vector<CBot*>::iterator iter = t_vClient.begin()
			; iter != t_vClient.end(); ++iter)
		{
			delete (*iter);
			t_vClient.erase(iter);
		}

		t_vClient.clear();
	//);

	return 1;
}