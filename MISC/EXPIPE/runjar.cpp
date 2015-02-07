#ifdef _WIN32
  #include <stdio.h>
  #include <windows.h>
#else
  #include <unistd.h>
#endif
#include "../../base/base2.h"

int main(void) {
  char *lpJreHome, *lpPath;
  char szJarFile[1024];

  LocatePath(szJarFile, "startup.jar");

#ifdef _WIN32

  char szCommand[1024];
  sprintf(szCommand, "java -jar \"%s\"", szJarFile);

  DWORD dw;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&si, 0, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  if (!CreateProcess(NULL, (LPSTR) szCommand, NULL, NULL, TRUE,
      (GetConsoleMode(si.hStdInput, &dw) ? 0 : DETACHED_PROCESS) |
      CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi)) {
    return -1;
  }
  WaitForSingleObject(pi.hProcess, INFINITE);
  GetExitCodeProcess(pi.hProcess, &dw);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return dw;

#else

  execlp("java", "java", "-jar", szJarFile, NULL);

#endif

}
