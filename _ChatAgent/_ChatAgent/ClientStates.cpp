#include "stdafx.h"

extern CLogWriter *g_pLog;

CLobby* CLobby::Instance()
{
	static CLobby instance;

	return &instance;
}

void CLobby::Enter(CClient *client)
{
	 g_pLog->myWprintf(L"Entered Lobby State\n");
}

void CLobby::Exit(CClient *client)
{
}

void CLobby::Execute(CClient *client, long long elapsed)
{

}

//---------------------------------------------

CRoom* CRoom::Instance()
{
	static CRoom instance;

	return &instance;
}

void CRoom::Enter(CClient *client)
{
	g_pLog->myWprintf(L"Entered Room State.\n");
}

void CRoom::Exit(CClient *client)
{
}

void CRoom::Execute(CClient *client, long long elapsed)
{
}

//---------------------------------------------

CDestroy_Response_Wait* CDestroy_Response_Wait::Instance()
{
	static CDestroy_Response_Wait instance;

	return &instance;
}

void CDestroy_Response_Wait::Enter(CClient *client)
{
	g_pLog->myWprintf(L"Entered Destroy Response State.\n");
	client->m_llAccumTime = 0;
}

void CDestroy_Response_Wait::Exit(CClient *client)
{
}

void CDestroy_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->m_llAccumTime += elapsed;
	if (client->m_llAccumTime > ELAPSED_DEST)
	{
		//client->SendDestroyMessage(client->m_nTmpRoom_num);

		client->m_llAccumTime = 0;
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
	g_pLog->myWprintf(L"Entered Join Response State");
	client->m_llAccumTime = 0;
}

void CJoin_Response_Wait::Exit(CClient *client)
{
}

void CJoin_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->m_llAccumTime += elapsed;
	if (client->m_llAccumTime > ELAPSED_DEST)
	{
		//client->SendJoinMessage(client->m_nTmpRoom_num, client->m_szTmpNickname);

		client->m_llAccumTime = 0;
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
	g_pLog->myWprintf(L"Entered Leave Response State");
	client->m_llAccumTime = 0;
}

void CLeave_Response_Wait::Exit(CClient *client)
{
}

void CLeave_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->m_llAccumTime += elapsed;
	if (client->m_llAccumTime > ELAPSED_DEST)
	{
		//client->SendLeaveMessage();

		client->m_llAccumTime = 0;
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
	g_pLog->myWprintf(L"Entered Create Response State");
	client->m_llAccumTime = 0;
}

void CCreate_Response_Wait::Exit(CClient *client)
{
}

void CCreate_Response_Wait::Execute(CClient *client, long long elapsed)
{
	client->m_llAccumTime += elapsed;
	if (client->m_llAccumTime > ELAPSED_DEST)
	{
		//client->SendCreateMessage(client->m_nTmpRoom_num);

		client->m_llAccumTime = 0;
	}
}

//---------------------------------------------

CEscaping* CEscaping::Instance()
{
	static CEscaping instance;

	return &instance;
}

void CEscaping::Enter(CClient *client)
{
	g_pLog->myWprintf(L"Entered Escaping State");
	client->m_llAccumTime = 0;
}

void CEscaping::Exit(CClient *client)
{
}

void CEscaping::Execute(CClient *client, long long elapsed)
{
	client->m_llAccumTime += elapsed;
	if (client->m_llAccumTime > ELAPSED_DEST)
	{
		//client->SendEscapeMessage();

		client->m_llAccumTime = 0;
	}
}