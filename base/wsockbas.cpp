#ifdef _WIN32

#include <windows.h>

#ifdef WSOCKBAS_DLL

extern "C" __declspec(dllexport) VOID WINAPI WSBStartup(VOID);
extern "C" __declspec(dllexport) VOID WINAPI WSBCleanup(VOID);
extern "C" __declspec(dllexport) LONG WINAPI WSBOpenServer(LONG nPort);
extern "C" __declspec(dllexport) VOID WINAPI WSBCloseServer(LONG nPort);
extern "C" __declspec(dllexport) LONG WINAPI WSBAccept(LONG nSocket);
extern "C" __declspec(dllexport) LONG WINAPI WSBConnect(LPCSTR lpszHost, LONG nPort);
extern "C" __declspec(dllexport) VOID WINAPI WSBDisconnect(LONG nSocket);
extern "C" __declspec(dllexport) LONG WINAPI WSBRecv(LONG nSocket, LPSTR lpBuffer, LONG nLen);
extern "C" __declspec(dllexport) LONG WINAPI WSBSend(LONG nSocket, LPCSTR lpBuffer, LONG nLen);

#endif

static u_long uNonBlock = 1;
static int nSockAddrLen = sizeof(sockaddr_in);

VOID WINAPI WSBStartup(VOID) {
  WSADATA wsaData;
  WSAStartup(0x101, &wsaData);
}

VOID WINAPI WSBCleanup(VOID) {
  WSACleanup();
}

LONG WINAPI WSBOpenServer(LONG nPort) {
  SOCKET s;
  sockaddr_in sa;
  s = socket(AF_INET, SOCK_STREAM, 0);
  sa.sin_family = AF_INET;
  sa.sin_port = htons((unsigned short) nPort);
  sa.sin_addr.s_addr = INADDR_ANY;
  if (bind(s, (sockaddr *) &sa, sizeof(sockaddr_in)) == 0) {
    listen(s, SOMAXCONN);
    ioctlsocket(s, FIONBIO, &uNonBlock);
    return (LONG) s;
  } else {
    closesocket(s);
    return INVALID_SOCKET;
  }
}

VOID WINAPI WSBCloseServer(LONG nSocket) {
  closesocket((SOCKET) nSocket);
}

LONG WINAPI WSBAccept(LONG nSocket) {
  SOCKET s;
  sockaddr_in sa;
  s = accept((SOCKET) nSocket, (sockaddr *) &sa, &nSockAddrLen);
  if (s != INVALID_SOCKET) {
    ioctlsocket(s, FIONBIO, &uNonBlock);
  }
  return (LONG) s;
}

LONG WINAPI WSBConnect(LPCSTR lpszHost, LONG nPort) {  
  SOCKET s;
  sockaddr_in sa;
  hostent *h;
  sa.sin_family = AF_INET;
  sa.sin_port = htons((unsigned short) nPort);
  h = gethostbyname(lpszHost);
  if (h == NULL) {
    sa.sin_addr.s_addr = inet_addr(lpszHost);
  } else {
    sa.sin_addr = *(in_addr *) h->h_addr;
  }
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(s, (sockaddr *) &sa, sizeof(sockaddr_in)) == 0) {
    ioctlsocket(s, FIONBIO, &uNonBlock);
    return (LONG) s;
  } else {
    closesocket(s);
    return INVALID_SOCKET;
  }
}

VOID WINAPI WSBDisconnect(LONG nSocket) {
  shutdown((SOCKET) nSocket, 2);
  closesocket((SOCKET) nSocket);
}

LONG WINAPI WSBRecv(LONG nSocket, LPSTR lpBuffer, LONG nLen) {
  int n;
  n = recv((SOCKET) nSocket, lpBuffer, nLen, 0);
  if (n < 0) {
    if (WSAGetLastError() == WSAEWOULDBLOCK) {
      return 0;
    } else {
      return -1;
    }
  } else if (n == 0) {
    return -1;
  } else {
    return n;
  }
}

LONG WINAPI WSBSend(LONG nSocket, LPCSTR lpBuffer, LONG nLen) {
  int n;
  n = send((SOCKET) nSocket, lpBuffer, nLen, 0);
  if (n < 0) {
    if (WSAGetLastError() == WSAEWOULDBLOCK) {
      return 0;
    } else {
      return -1;
    }
  } else {
    return n;
  }
}

#else

#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

const int INVALID_SOCKET = -1;

static u_long uNonBlock = 1;
static socklen_t nSockAddrLen = sizeof(sockaddr_in);

int WSBOpenServer(int nPort) {
  int s;
  sockaddr_in sa;
  s = socket(AF_INET, SOCK_STREAM, 0);
  sa.sin_family = AF_INET;
  sa.sin_port = htons((unsigned short) nPort);
  sa.sin_addr.s_addr = INADDR_ANY;
  if (bind(s, (sockaddr *) &sa, sizeof(sockaddr_in)) == 0) {
    listen(s, SOMAXCONN);
    ioctl(s, FIONBIO, &uNonBlock);
    return s;
  } else {
    close(s);
    return INVALID_SOCKET;
  }
}

void WSBCloseServer(int nSocket) {
  close(nSocket);
}

int WSBAccept(int nSocket) {
  int s;
  sockaddr_in sa;
  s = accept(nSocket, (sockaddr *) &sa, &nSockAddrLen);
  if (s != INVALID_SOCKET) {
    ioctl(s, FIONBIO, &uNonBlock);
  }
  return s;
}

int WSBConnect(const char *lpszHost, int nPort) {  
  int s;
  sockaddr_in sa;
  hostent *h;
  sa.sin_family = AF_INET;
  sa.sin_port = htons((unsigned short) nPort);
  h = gethostbyname(lpszHost);
  if (h == NULL) {
    sa.sin_addr.s_addr = inet_addr(lpszHost);
  } else {
    sa.sin_addr = *(in_addr *) h->h_addr;
  }
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(s, (sockaddr *) &sa, sizeof(sockaddr_in)) == 0) {
    ioctl(s, FIONBIO, &uNonBlock);
    return s;
  } else {
    close(s);
    return INVALID_SOCKET;
  }
}

void WSBDisconnect(int nSocket) {
  shutdown(nSocket, 2);
  close(nSocket);
}

int WSBRecv(int nSocket, char *lpBuffer, int nLen) {
  int n;
  n = recv(nSocket, lpBuffer, nLen, 0);
  if (n < 0) {
    if (errno == EWOULDBLOCK) {
      return 0;
    } else {
      return -1;
    }
  } else if (n == 0) {
    return -1;
  } else {
    return n;
  }
}

int WSBSend(int nSocket, const char *lpBuffer, int nLen) {
  int n;
  n = send(nSocket, lpBuffer, nLen, 0);
  if (n < 0) {
    if (errno == EWOULDBLOCK) {
      return 0;
    } else {
      return -1;
    }
  } else {
    return n;
  }
}

#endif
