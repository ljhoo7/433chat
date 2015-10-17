#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

extern bool g_bExitSignal;

CLobby* CLobby::Instance()
{
	static CLobby instance;

	return &instance;
}

void CLobby::Enter(CConnection *client)
{
	std::cout << "Entered Lobby State" << std::endl;
}

void CLobby::Exit(CConnection *client)
{
}

void CLobby::Execute(CConnection *client, long long elapsed)
{
	
}

//---------------------------------------------

CRoom* CRoom::Instance()
{
	static CRoom instance;

	return &instance;
}

void CRoom::Enter(CConnection *client)
{
	std::cout << "Entered Room State" << std::endl;
}

void CRoom::Exit(CConnection *client)
{
}

void CRoom::Execute(CConnection *client, long long elapsed)
{
}

//---------------------------------------------

CDestroy_Response_Wait* CDestroy_Response_Wait::Instance()
{
	static CDestroy_Response_Wait instance;

	return &instance;
}

void CDestroy_Response_Wait::Enter(CConnection *client)
{
	std::cout << "Entered Destroy Response State" << std::endl;
	client->accumTime = 0;
}

void CDestroy_Response_Wait::Exit(CConnection *client)
{
}

void CDestroy_Response_Wait::Execute(CConnection *client, long long elapsed)
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

void CJoin_Response_Wait::Enter(CConnection *client)
{
	std::cout << "Entered Join Response State" << std::endl;
	client->accumTime = 0;
}

void CJoin_Response_Wait::Exit(CConnection *client)
{
}

void CJoin_Response_Wait::Execute(CConnection *client, long long elapsed)
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

void CLeave_Response_Wait::Enter(CConnection *client)
{
	std::cout << "Entered Leave Response State" << std::endl;
	client->accumTime = 0;
}

void CLeave_Response_Wait::Exit(CConnection *client)
{
}

void CLeave_Response_Wait::Execute(CConnection *client, long long elapsed)
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

void CCreate_Response_Wait::Enter(CConnection *client)
{
	std::cout << "Entered Create Response State" << std::endl;
	client->accumTime = 0;
}

void CCreate_Response_Wait::Exit(CConnection *client)
{
}

void CCreate_Response_Wait::Execute(CConnection *client, long long elapsed)
{
	client->accumTime += elapsed;
	if (client->accumTime > ELAPSED_DEST)
	{
		client->SendCreateMessage(client->m_nTmpRoom_num);

		client->accumTime = 0;
	}
}