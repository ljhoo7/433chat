
#define CRTDBG_MAP_ALLOC 
#include "stdafx.h"
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__ )   

#endif
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 4)
	{
		PRINTF("The Number of Parameters is error !\n");

		exit(0);
	}

	unsigned short agentPort = _wtoi(argv[3]);

	/// server & monitoring server IP & PORT INPUT
	unsigned int gMonitoringServerPort = _wtoi(argv[1]);
	DWORD gMonitoringServerIP		   = _tinet_addr(argv[2]);

	if (!(1024 < gMonitoringServerPort && gMonitoringServerPort < 66535))
	{
		PRINTF("The Monitoring Server Port Number is Invalid !\n");

		exit(0);
	}

	AgentApp::Instance()->Init(agentPort, gMonitoringServerIP, gMonitoringServerPort);
	
	while (!(AgentApp::Instance()->IsExit()))
	{
		AgentApp::Instance()->Run();
	}
	
	AgentApp::Instance()->Destroy();
	_CrtDumpMemoryLeaks();
	return 0;
}

