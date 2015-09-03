#include "Headers.h"
#include "..\Utilities.h"

bool g_bExitSignal;

int main(int argc, char *argv[])
{
	g_bExitSignal = false;

	CHECK_FAILURE(
	std::vector<CClient*> t_vClient;

	// Allocate a client
	CHECK_FAILURE(t_vClient.push_back(new CClient()));

	// Keep Looping for each clients
	while (!g_bExitSignal){
		for (std::vector<CClient*>::iterator iter = t_vClient.begin()
			; iter != t_vClient.end(); ++iter)
		{
			(*iter)->GetStateMachine()->Update();
		}
	}

	// Wait for multiple threads
	for (std::vector<CClient*>::iterator iter = t_vClient.begin()
		; iter != t_vClient.end(); ++iter)
	{
		(*iter)->GetReceivingThread()->join();
	}

	std::cout << "All thread has been joined." << std::endl;

	// Deallocating all
	for (std::vector<CClient*>::iterator iter = t_vClient.begin()
		; iter != t_vClient.end(); ++iter)
	{
		delete (*iter);
		t_vClient.erase(iter);
	}
	);

	return 1;
}