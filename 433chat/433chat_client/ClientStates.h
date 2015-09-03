#pragma once

class CNotConnected : public State<CClient>
{
private:
	CNotConnected(){}
	CNotConnected(const CNotConnected&);
	CNotConnected& operator=(const CNotConnected&);
public:
	static CNotConnected* Instance();
	virtual void Enter(CClient& client);
	virtual void Exit(CClient& client);
	virtual void Execute(CClient& client);
};

class CLobby : public State<CClient>
{
private:
	CLobby(){}
	CLobby(const CLobby&);
	CLobby& operator=(const CLobby&);
public:
	static CLobby* Instance();
	virtual void Enter(CClient& client);
	virtual void Exit(CClient& client);
	virtual void Execute(CClient& client);
};

class CRoom : public State<CClient>
{
private:
	CRoom(){}
	CRoom(const CRoom&);
	CRoom& operator=(const CRoom&);
public:
	static CRoom* Instance();
	virtual void Enter(CClient& client);
	virtual void Exit(CClient& client);
	virtual void Execute(CClient& client);
};