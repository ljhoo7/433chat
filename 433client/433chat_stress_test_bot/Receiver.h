#pragma once

class CReceiver
{
public:
	CReceiver(void);
	~CReceiver(void);

	bool MyReceiveMessage(t_packet& tmppacket, const int retval);
};

