#pragma once

class CProactor
{
public:
	mswsock_s					m_cMswsock;
private:
	std::vector<CBot*>			m_vBot;

	DWORD						m_dwIp;
	int							m_nPort;
	int							m_nBotCnt;
	int							m_nThreadCnt;

	HANDLE						m_hIOCP;

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