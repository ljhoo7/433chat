#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

char randStr[RANDSTR_SIZE][RANDSTR_LENG] =
{ "Hello World ~ ", "Hi Hi !!", "Nice to meet you", "I'm a programmer.", "So what ?"
, "Good luck", "Good morning ~" };

extern bool g_bExitSignal;

CRoom* CRoom::Instance()
{
	static CRoom instance;

	return &instance;
}

void CRoom::Enter(CBot *bot)
{
}

void CRoom::Exit(CBot *bot)
{
}

void CRoom::Execute(CBot *bot, long long elapsed)
{
	bot->SendChatMessage(randStr[elapsed % RANDSTR_SIZE]);
}

//-----------------------------------------

CLobby* CLobby::Instance()
{
	static CLobby instance;

	return &instance;
}

void CLobby::Enter(CBot *bot)
{
}

void CLobby::Exit(CBot *bot)
{
}

void CLobby::Execute(CBot *bot, long long elapsed)
{
}

//---------------------------------------------

CJoin_Response_Wait* CJoin_Response_Wait::Instance()
{
	static CJoin_Response_Wait instance;

	return &instance;
}

void CJoin_Response_Wait::Enter(CBot *bot)
{
	bot->accumTime = 0;
}

void CJoin_Response_Wait::Exit(CBot *bot)
{
}

void CJoin_Response_Wait::Execute(CBot *bot, long long elapsed)
{
	bot->accumTime += elapsed;
	if (bot->accumTime > ELAPSED_DEST)
	{
		bot->SendJoinMessage(bot->m_nTmpRoom_num, bot->m_vcNickname);

		bot->accumTime = 0;
	}
}

//---------------------------------------------

CCreate_Response_Wait* CCreate_Response_Wait::Instance()
{
	static CCreate_Response_Wait instance;

	return &instance;
}

void CCreate_Response_Wait::Enter(CBot *bot)
{
	bot->accumTime = 0;
}

void CCreate_Response_Wait::Exit(CBot *bot)
{
}

void CCreate_Response_Wait::Execute(CBot *bot, long long elapsed)
{
	bot->accumTime += elapsed;
	if (bot->accumTime > ELAPSED_DEST)
	{
		bot->SendCreateMessage(bot->m_nTmpRoom_num);

		bot->accumTime = 0;
	}
}