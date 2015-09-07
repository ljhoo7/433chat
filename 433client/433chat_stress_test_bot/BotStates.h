#pragma once

class CRoom : public State<CBot>
{
private:
	CRoom(){}
	CRoom(const CRoom&);
	CRoom& operator=(const CRoom&);
public:
	static CRoom* Instance();
	virtual void Enter(CBot *client);
	virtual void Exit(CBot *client);
	virtual void Execute(CBot *client, long long);
};

class CLobby : public State<CBot>
{
private:
	CLobby(){}
	CLobby(const CLobby&);
	CLobby& operator=(const CLobby&);
public:
	static CLobby* Instance();
	virtual void Enter(CBot *client);
	virtual void Exit(CBot *client);
	virtual void Execute(CBot *client, long long);
};

class CCreate_Response_Wait : public State<CBot>
{
private:
	CCreate_Response_Wait(){}
	CCreate_Response_Wait(const CCreate_Response_Wait&);
	CCreate_Response_Wait& operator=(const CCreate_Response_Wait&);
public:
	static CCreate_Response_Wait* Instance();
	virtual void Enter(CBot *client);
	virtual void Exit(CBot *client);
	virtual void Execute(CBot *client, long long);
};

class CJoin_Response_Wait : public State<CBot>
{
private:
	CJoin_Response_Wait(){}
	CJoin_Response_Wait(const CJoin_Response_Wait&);
	CJoin_Response_Wait& operator=(const CJoin_Response_Wait&);
public:
	static CJoin_Response_Wait* Instance();
	virtual void Enter(CBot *client);
	virtual void Exit(CBot *client);
	virtual void Execute(CBot *client, long long);
};