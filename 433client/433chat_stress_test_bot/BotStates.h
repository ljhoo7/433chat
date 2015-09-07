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
	virtual void Execute(CBot *client);
};