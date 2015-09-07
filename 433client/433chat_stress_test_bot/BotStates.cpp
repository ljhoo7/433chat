#include "Predeclaration.h"
#include "..\Utilities.h"
#include "Headers.h"

extern bool g_bExitSignal;

CRoom* CRoom::Instance()
{
	static CRoom instance;

	return &instance;
}

void CRoom::Enter(CBot *bot)
{
	std::cout << "Entered Room State" << std::endl;
}

void CRoom::Exit(CBot *bot)
{
}

void CRoom::Execute(CBot *bot)
{
	bot->SendChatMessage("Hello");
}
