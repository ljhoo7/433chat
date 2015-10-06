#pragma once

// Print Socket Error and quit
void Err_quit(char *msg);

// Print Socket Error
void Err_display(char *msg);

void PrintFailSignal(const fail_signal fail);

DWORD _tinet_addr(const TCHAR *cp);

void BeforeMakingIOCPMessage(PTCH p_szStr);