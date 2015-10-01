#pragma once

// socket func error print and exit
void err_quit(char *msg);

// socket func error print only
void err_display(char *msg);

// custom define recieve func
int recvn(SOCKET s, char *buf, int len, int flags);