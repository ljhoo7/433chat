#include "Headers.h"
#include "..\Utilities.h"

int main(int argc, char *argv[])
{
	CHECK_FAILURE(
	std::vector<CClient*> t_vClient;

	// Allocate a client
	CHECK_FAILURE(t_vClient.push_back(new CClient()));

	// Keep Looping for each clients
	while (1){
		for (std::vector<CClient*>::iterator iter = t_vClient.begin()
			; iter != t_vClient.end(); ++iter)
		{
			(*iter)->GetStateMachine()->CurrentState()->Execute(**iter);
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