#pragma once

struct CClient
{
	CConnection *connections[2];

	//-----------------------------------------------------

	CClient();
	~CClient();

	void Update(long long elapsed);
};