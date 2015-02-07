#ifndef WSOCKBAS_H
#define WSOCKBAS_H

#ifdef _WIN32

#include <windows.h>

VOID WINAPI WSBStartup(VOID);
VOID WINAPI WSBCleanup(VOID);
LONG WINAPI WSBOpenServer(LONG nPort);
VOID WINAPI WSBCloseServer(LONG nSocket);
LONG WINAPI WSBAccept(LONG nSocket);
LONG WINAPI WSBConnect(LPCSTR lpszHost, LONG nPort);
VOID WINAPI WSBDisconnect(LONG nSocket);
LONG WINAPI WSBRecv(LONG nSocket, LPSTR lpBuffer, LONG nLen);
LONG WINAPI WSBSend(LONG nSocket, LPCSTR lpBuffer, LONG nLen);

#else

const int INVALID_SOCKET = -1;

inline void WSBStartup(void) {};
inline void WSBCleanup(void) {};
int WSBOpenServer(int nPort);
void WSBCloseServer(int nSocket);
int WSBAccept(int nSocket);
int WSBConnect(const char *lpszHost, int nPort);
void WSBDisconnect(int nSocket);
int WSBRecv(int nSocket, char *lpBuffer, int nLen);
int WSBSend(int nSocket, const char *lpBuffer, int nLen);

#endif

#endif
