#pragma once

struct CConnection
{
	char nickname[NICK_SIZE];
	int m_nRoom_num;
	unsigned int m_nToken;

	std::string buf_str;
	long long accumTime;

	// This is the temporary room number until the time when this client receives the success packet.
	int m_nTmpRoom_num;
	// This is the temporary nickname until the time when this client receives the success packet.
	char m_vcNickname[NICK_SIZE];

	SOCKET sock;

	std::thread						*m_pReceivingThread;
	StateMachine<CConnection>		*m_pStateMachine;

	void ReceivingThread();

	StateMachine<CConnection>		*GetStateMachine()
	{
		if (m_pStateMachine == nullptr)
		{
			CHECK_FAILURE(std::cout << "Error : m_pStateMachine is nullptr !" << std::endl);

			return NULL;
		}
		else
		{
			return m_pStateMachine;
		}
	}

	bool SetNickName(char *param)
	{
		if (strlen(param) > 19)
			return false;
		strcpy_s(nickname, param);
		return true;
	}

	std::thread					*GetReceivingThread()					{ return m_pReceivingThread; }

	CConnection(unsigned long ip, unsigned short port, char* nick = NULL, int rNum = -1);
	~CConnection();

	bool						SendCreateMessage(int num);
	bool						SendDestroyMessage(int num);
	bool						SendJoinMessage(int num, char *nick);
	bool						SendLeaveMessage();
	bool						SendChatMessage(const std::string& str);

	bool						Escaping(unsigned long destIp, unsigned short port);
};