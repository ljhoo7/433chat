#pragma once

class CProactor
{
public:
	mswsock_s					m_cMswsock;
	int							m_nBotCnt;
	HANDLE						m_hIOCP, *m_hEventForAllJoin;

	std::vector<CBot*>			m_vBot;
private:
	DWORD						m_dwIp;
	int							m_nPort;
	
	int							m_nThreadCnt;

	CReceiver					*m_pReceiver;
	CSender						*m_pSender;

	CConnector					*m_pConnector;
	CDisconnector				*m_pDisconnector;
public:
	CProactor(DWORD p_dwIp, int p_nPort, int p_nBotCnt, int p_nThreadCnt);
	~CProactor();
static UINT WINAPI ThreadFunc(PVOID p_pParam);
	

	bool ProcEvent();
	bool Register(HANDLE m_hHandle);

	bool Init();
};