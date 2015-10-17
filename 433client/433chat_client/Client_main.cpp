#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

bool g_bExitSignal;

extern int t_cnt;

int main(int argc, char *argv[])
{
	// initializing the windows socket
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NULL)
	{
		std::cout << "There is an error on WSAStartup() << std::endl";

		exit(1);
	}

	static std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tmp;
	std::chrono::milliseconds tmp2;

	int fps = 30;
	double block = 1000 / fps;
	long long time;
	long long prevTime = 0;

	g_bExitSignal = false;

	std::vector<CClient*> t_vClient;

	// Allocate a client
	t_vClient.push_back(new CClient());

	// Keep Looping for each clients
	while (!g_bExitSignal)
	{
		tmp = std::chrono::system_clock::now() - start_time;
		tmp2 = std::chrono::duration_cast<std::chrono::milliseconds>(tmp);
		time = tmp2.count();
		if (block <= time)
		{
			for (std::vector<CClient*>::iterator iter = t_vClient.begin()
				; iter != t_vClient.end(); ++iter)
			{
				(*iter)->Update(time - prevTime);
			}

			start_time = std::chrono::system_clock::now();
			time = 0;
		}

		prevTime = time;
	}

	// Wait for multiple threads
	for (std::vector<CClient*>::iterator iter = t_vClient.begin()
		; iter != t_vClient.end(); ++iter)
	{
		if (NULL != (*iter)->connections[0])
			(*iter)->connections[0]->GetReceivingThread()->join();
		if (NULL != (*iter)->connections[1])
			(*iter)->connections[1]->GetReceivingThread()->join();
	}

	std::cout << "All thread has been joined." << std::endl;

	// Deallocating all
	for (std::vector<CClient*>::iterator iter = t_vClient.begin()
		; iter != t_vClient.end(); ++iter)
	{
		delete (*iter);
		t_vClient.erase(iter);
	}

	// Windows sock close
	if (WSACleanup() != 0)
	{
		std::cout << "An error occured on WASCleanup()." << std::endl;
		exit(1);
	}

	return 1;
}