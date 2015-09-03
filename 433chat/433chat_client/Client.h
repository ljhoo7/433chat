#pragma once

class CClient
{
	SOCKET sock;

	std::thread *m_pReceivingThread;

	char nickname[NICK_SIZE];

	int m_nRoom_num;

	// This is the temporary room number until the time when this client receives the success packet.
	int m_nTmpJoinRoom_num;

	unsigned int m_nToken;

	HANDLE hReceiving;

	StateMachine<CClient>								*m_pStateMachine;
public:
	CClient();
	~CClient();

	void ReceivingThread();

	StateMachine<CClient>		*GetStateMachine()
	{
		if (m_pStateMachine == nullptr)
		{
			CHECK_FAILURE(std::cout << "Error : m_pStateMachine is nullptr !" << std::endl);
		}
		else
		{
			return m_pStateMachine;
		}
	}

	std::thread					*GetReceivingThread()					{ return m_pReceivingThread; }

	int							GetRoomNumber()							{ return m_nRoom_num; }
	int							GetTmpJoinRoomNumber()					{ return m_nTmpJoinRoom_num; }
	char*						GetNickName()							{ return nickname; }
	SOCKET						GetSocket()								{ return sock; }
	unsigned int				GetToken()								{ return m_nToken; }

	void						SetRoomNumber(int param)				{ m_nRoom_num = param; }
	void						SetTmpJoinRoomNumber(int param)			{ m_nTmpJoinRoom_num = param; }
	void						SetToken(unsigned int param)			{ m_nToken = param; }

	bool						SendCreateMessage(int num);
	bool						SendDestroyMessage(int num);
	bool						SendJoinMessage(int num, char *nick);
	bool						SendLeaveMessage();
	bool						SendChatMessage(const std::string& str);
};