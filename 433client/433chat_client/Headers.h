#pragma once

// Macro functions
#ifdef DEBUG
#define CHECK_FAILURE(X) \
	try\
{\
	X\
}\
	catch (std::iostream::failure &e)\
{\
	std::cout << "Error Code : " << e.code << std::endl\
	<< "Error Message : " << e.what << std::endl; \
	exit(1);\
}
#else
#define CHECK_FAILURE(X) X
#endif

#include "..\Protocol.h"
#include "ClientStates.h"
#include "Client.h"
#include "Connection.h"