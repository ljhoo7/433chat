#pragma once

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

class CDestroy_Response_Wait : public State<CBot>
{
private:
	CDestroy_Response_Wait(){}
	CDestroy_Response_Wait(const CDestroy_Response_Wait&);
	CDestroy_Response_Wait& operator=(const CDestroy_Response_Wait&);
public:
	static CDestroy_Response_Wait* Instance();
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

class CLeave_Response_Wait : public State<CBot>
{
private:
	CLeave_Response_Wait(){}
	CLeave_Response_Wait(const CLeave_Response_Wait&);
	CLeave_Response_Wait& operator=(const CLeave_Response_Wait&);
public:
	static CLeave_Response_Wait* Instance();
	virtual void Enter(CBot *client);
	virtual void Exit(CBot *client);
	virtual void Execute(CBot *client, long long);
};

class CEscaping : public State<CBot>
{
private:
	CEscaping(){}
	CEscaping(const CEscaping&);
	CEscaping& operator=(const CEscaping&);
public:
	static CEscaping* Instance();
	virtual void Enter(CBot *client);
	virtual void Exit(CBot *client);
	virtual void Execute(CBot *client, long long);
};