#pragma once

class CLobby : public State<CConnection>
{
private:
	CLobby(){}
	CLobby(const CLobby&);
	CLobby& operator=(const CLobby&);
public:
	static CLobby* Instance();
	virtual void Enter(CConnection *client);
	virtual void Exit(CConnection *client);
	virtual void Execute(CConnection *client, long long);
};

class CRoom : public State<CConnection>
{
private:
	CRoom(){}
	CRoom(const CRoom&);
	CRoom& operator=(const CRoom&);
public:
	static CRoom* Instance();
	virtual void Enter(CConnection *client);
	virtual void Exit(CConnection *client);
	virtual void Execute(CConnection *client, long long);
};

class CCreate_Response_Wait : public State<CConnection>
{
private:
	CCreate_Response_Wait(){}
	CCreate_Response_Wait(const CCreate_Response_Wait&);
	CCreate_Response_Wait& operator=(const CCreate_Response_Wait&);
public:
	static CCreate_Response_Wait* Instance();
	virtual void Enter(CConnection *client);
	virtual void Exit(CConnection *client);
	virtual void Execute(CConnection *client, long long);
};

class CDestroy_Response_Wait : public State<CConnection>
{
private:
	CDestroy_Response_Wait(){}
	CDestroy_Response_Wait(const CDestroy_Response_Wait&);
	CDestroy_Response_Wait& operator=(const CDestroy_Response_Wait&);
public:
	static CDestroy_Response_Wait* Instance();
	virtual void Enter(CConnection *client);
	virtual void Exit(CConnection *client);
	virtual void Execute(CConnection *client, long long);
};

class CJoin_Response_Wait : public State<CConnection>
{
private:
	CJoin_Response_Wait(){}
	CJoin_Response_Wait(const CJoin_Response_Wait&);
	CJoin_Response_Wait& operator=(const CJoin_Response_Wait&);
public:
	static CJoin_Response_Wait* Instance();
	virtual void Enter(CConnection *client);
	virtual void Exit(CConnection *client);
	virtual void Execute(CConnection *client, long long);
};

class CLeave_Response_Wait : public State<CConnection>
{
private:
	CLeave_Response_Wait(){}
	CLeave_Response_Wait(const CLeave_Response_Wait&);
	CLeave_Response_Wait& operator=(const CLeave_Response_Wait&);
public:
	static CLeave_Response_Wait* Instance();
	virtual void Enter(CConnection *client);
	virtual void Exit(CConnection *client);
	virtual void Execute(CConnection *client, long long);
};