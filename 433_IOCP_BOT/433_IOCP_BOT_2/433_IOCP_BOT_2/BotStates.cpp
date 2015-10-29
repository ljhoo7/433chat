#include "stdafx.h"

extern CLogWriter *g_pLog;

CLobby* CLobby::Instance()
{
	static CLobby instance;

	return &instance;
}

void CLobby::Enter(CBot *p_pBot)
{
	 g_pLog->myWprintf("Entered Lobby State\n");
}

void CLobby::Exit(CBot *p_pBot)
{
}

void CLobby::Execute(CBot *p_pBot, long long elapsed)
{

}

//---------------------------------------------

CRoom* CRoom::Instance()
{
	static CRoom instance;

	return &instance;
}

void CRoom::Enter(CBot *p_pBot)
{
	g_pLog->myWprintf("Entered Room State.\n");
}

void CRoom::Exit(CBot *p_pBot)
{
}

void CRoom::Execute(CBot *p_pBot, long long elapsed)
{
}

//---------------------------------------------

CDestroy_Response_Wait* CDestroy_Response_Wait::Instance()
{
	static CDestroy_Response_Wait instance;

	return &instance;
}

void CDestroy_Response_Wait::Enter(CBot *p_pBot)
{
	g_pLog->myWprintf("Entered Destroy Response State.\n");
	p_pBot->m_llAccumTime = 0;
}

void CDestroy_Response_Wait::Exit(CBot *p_pBot)
{
}

void CDestroy_Response_Wait::Execute(CBot *p_pBot, long long elapsed)
{
	p_pBot->m_llAccumTime += elapsed;
	if (p_pBot->m_llAccumTime > ELAPSED_DEST)
	{
		//p_pBot->SendDestroyMessage(p_pBot->m_nTmpRoom_num);

		p_pBot->m_llAccumTime = 0;
	}
}

//---------------------------------------------

CJoin_Response_Wait* CJoin_Response_Wait::Instance()
{
	static CJoin_Response_Wait instance;

	return &instance;
}

void CJoin_Response_Wait::Enter(CBot *p_pBot)
{
	g_pLog->myWprintf("Entered Join Response State");
	p_pBot->m_llAccumTime = 0;
}

void CJoin_Response_Wait::Exit(CBot *p_pBot)
{
}

void CJoin_Response_Wait::Execute(CBot *p_pBot, long long elapsed)
{
	p_pBot->m_llAccumTime += elapsed;
	if (p_pBot->m_llAccumTime > ELAPSED_DEST)
	{
		//p_pBot->SendJoinMessage(p_pBot->m_nTmpRoom_num, p_pBot->m_szTmpNickname);

		p_pBot->m_llAccumTime = 0;
	}
}

//---------------------------------------------

CLeave_Response_Wait* CLeave_Response_Wait::Instance()
{
	static CLeave_Response_Wait instance;

	return &instance;
}

void CLeave_Response_Wait::Enter(CBot *p_pBot)
{
	g_pLog->myWprintf("Entered Leave Response State");
	p_pBot->m_llAccumTime = 0;
}

void CLeave_Response_Wait::Exit(CBot *p_pBot)
{
}

void CLeave_Response_Wait::Execute(CBot *p_pBot, long long elapsed)
{
	p_pBot->m_llAccumTime += elapsed;
	if (p_pBot->m_llAccumTime > ELAPSED_DEST)
	{
		//p_pBot->SendLeaveMessage();

		p_pBot->m_llAccumTime = 0;
	}
}

//---------------------------------------------

CCreate_Response_Wait* CCreate_Response_Wait::Instance()
{
	static CCreate_Response_Wait instance;

	return &instance;
}

void CCreate_Response_Wait::Enter(CBot *p_pBot)
{
	g_pLog->myWprintf("Entered Create Response State");
	p_pBot->m_llAccumTime = 0;
}

void CCreate_Response_Wait::Exit(CBot *p_pBot)
{
}

void CCreate_Response_Wait::Execute(CBot *p_pBot, long long elapsed)
{
	p_pBot->m_llAccumTime += elapsed;
	if (p_pBot->m_llAccumTime > ELAPSED_DEST)
	{
		//p_pBot->SendCreateMessage(p_pBot->m_nTmpRoom_num);

		p_pBot->m_llAccumTime = 0;
	}
}

//---------------------------------------------

CEscaping* CEscaping::Instance()
{
	static CEscaping instance;

	return &instance;
}

void CEscaping::Enter(CBot *p_pBot)
{
	g_pLog->myWprintf("Entered Escaping State");
	p_pBot->m_llAccumTime = 0;
}

void CEscaping::Exit(CBot *p_pBot)
{
}

void CEscaping::Execute(CBot *p_pBot, long long elapsed)
{
	p_pBot->m_llAccumTime += elapsed;
	if (p_pBot->m_llAccumTime > ELAPSED_DEST)
	{
		//p_pBot->SendEscapeMessage();

		p_pBot->m_llAccumTime = 0;
	}
}