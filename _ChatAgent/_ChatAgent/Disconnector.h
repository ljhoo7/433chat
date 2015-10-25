#pragma once


class Disconnector : public Actor
{
public:
	Disconnector(Proactor *p_pProactor);
	~Disconnector();

	void ProcEvent(Act* act, DWORD bytes_transferred);
	void ProcError(Act* act, DWORD error);
};

