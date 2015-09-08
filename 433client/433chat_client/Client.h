#pragma once

class CClient
{
	SOCKET sock;

	std::thread *m_pReceivingThread;

	char nickname[NICK_SIZE];

	int m_nRoom_num;

	unsigned int m_nToken;

	HANDLE hReceiving;

	StateMachine<CClient>								*m_pStateMachine;
public:
	// This is the temporary room number until the time when this client receives the success packet.
	int m_nTmpRoom_num;
	// This is the temporary nickname until the time when this client receives the success packet.
	char m_vcNickname[NICK_SIZE];

	std::string buf_str;
	long long accumTime;

	//-----------------------------------------------------

	CClient();
	~CClient();

	void ReceivingThread();

	StateMachine<CClient>		*GetStateMachine()
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

	void printPrompt();

	std::thread					*GetReceivingThread()					{ return m_pReceivingThread; }

	int							GetRoomNumber()							{ return m_nRoom_num; }
	int							GetTmpJoinRoomNumber()					{ return m_nTmpRoom_num; }
	char*						GetNickName()							{ return nickname; }
	SOCKET						GetSocket()								{ return sock; }
	unsigned int				GetToken()								{ return m_nToken; }

	void						SetRoomNumber(int param)				{ m_nRoom_num = param; }
	void						SetTmpJoinRoomNumber(int param)			{ m_nTmpRoom_num = param; }
	void						SetToken(unsigned int param)			{ m_nToken = param; }

	bool						SetNickName(char *param)
	{
		if (strlen(param) > 19)
			return false;
		strcpy_s(nickname, param);
		return true;
	}

	bool						SendCreateMessage(int num);
	bool						SendDestroyMessage(int num);
	bool						SendJoinMessage(int num, char *nick);
	bool						SendLeaveMessage();
	bool						SendChatMessage(const std::string& str);
};