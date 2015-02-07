#ifdef _WIN32
  #include <windows.h>
#else
  #include <pthread.h>
  #include <stdlib.h>
  #include <unistd.h>
#endif
#include <string.h>
#include "base.h"

#ifndef BASE2_H
#define BASE2_H

const int PATH_MAX_CHAR = 1024;

#ifdef _WIN32

inline void Idle(void) {
  Sleep(1);
}

const int PATH_SEPARATOR = '\\';

inline bool AbsolutePath(const char *sz) {
  return sz[0] == '\\' || (((sz[0] >= 'A' && sz[0] <= 'Z') || (sz[0] >= 'a' && sz[0] <= 'z')) && sz[1] == ':');
}

inline void GetSelfExe(char *szDst) {
  GetModuleFileName(NULL, szDst, PATH_MAX_CHAR);
}

inline void StartThread(void *ThreadEntry(void *), void *lpParameter) {
  DWORD dwThreadId;
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadEntry, (LPVOID) lpParameter, 0, &dwThreadId);
}

#else

inline void Idle(void) {
  usleep(1000);
}

const int PATH_SEPARATOR = '/';

inline bool AbsolutePath(const char *sz) {
  return sz[0] == '/' || (sz[0] == '~' && sz[1] == '/');
}

inline void GetSelfExe(char *szDst) {
  readlink("/proc/self/exe", szDst, PATH_MAX_CHAR);
}

inline void StartThread(void *ThreadEntry(void *), void *lpParameter) {
  pthread_t pthread;
  pthread_attr_t pthread_attr;
  pthread_attr_init(&pthread_attr);
  pthread_attr_setscope(&pthread_attr, PTHREAD_SCOPE_SYSTEM);
  pthread_create(&pthread, &pthread_attr, ThreadEntry, lpParameter);
}

#endif

inline void LocatePath(char *szDst, const char *szSrc) {
  char *lpSeparator;
  if (AbsolutePath(szSrc)) {
    strcpy(szDst, szSrc);
  } else {
    GetSelfExe(szDst);
    lpSeparator = strrchr(szDst, PATH_SEPARATOR);
    if (lpSeparator == NULL) {
      strcpy(szDst, szSrc);
    } else {
      strcpy(lpSeparator + 1, szSrc);
    }
  }
}

#endif
