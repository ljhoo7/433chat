#pragma once

class CClient
{
	SOCKET sock;

	std::thread *m_pReceivingThread;

	char nickname[NICK_SIZE];
	int room_num;

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

	std::thread					*GetReceivingThread()	{ return m_pReceivingThread; }

	int							GetRoomNumber()			{ return room_num; }
	char*						GetNickName()			{ return nickname; }

	bool						SendCreateMessage(int num);
	bool						SendDestroyMessage(int num);
	bool						SendJoinMessage(int num, char *nick);
	bool						SendLeaveMessage();
	bool						SendChatMessage(char *str);
};