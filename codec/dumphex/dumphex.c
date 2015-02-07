#include <string.h>

#ifdef DUMPHEX_DLL

#include <windows.h>
#include "dumphex.h"

__declspec(dllexport) VOID WINAPI DumpHexA(LPSTR szHexText, LPCSTR lpBuffer, LONG nOffset, LONG nLength);

VOID WINAPI DumpHexA(LPSTR szHexText, LPCSTR lpBuffer, LONG nOffset, LONG nLength) {
  DumpHex(szHexText, lpBuffer, nOffset, nLength);
}

#endif

static const char *const cszHexChar = "0123456789ABCDEF";

static int DumpLine(char *szHexText, const char *lpBuffer, int nOffsetDiv16, int nBegin, int nEnd) {
  int i, nByte;

  memset(szHexText, ' ', 77);
  szHexText[77] = '\r';
  szHexText[78] = '\n';
  szHexText[0] = cszHexChar[(nOffsetDiv16 >> 24) & 0xf];
  szHexText[1] = cszHexChar[(nOffsetDiv16 >> 20) & 0xf];
  szHexText[2] = cszHexChar[(nOffsetDiv16 >> 16) & 0xf];
  szHexText[3] = cszHexChar[(nOffsetDiv16 >> 12) & 0xf];
  szHexText[4] = ':';
  szHexText[5] = cszHexChar[(nOffsetDiv16 >> 8) & 0xf];
  szHexText[6] = cszHexChar[(nOffsetDiv16 >> 4) & 0xf];
  szHexText[7] = cszHexChar[nOffsetDiv16 & 0xf];
  szHexText[8] = '0';
  for (i = 0; i < 16; i ++) {
    if (i >= nBegin && i < nEnd) {
      nByte = lpBuffer[nOffsetDiv16 * 16 + i];
      szHexText[i * 3 + 11] = cszHexChar[(nByte & 0xf0) >> 4];
      szHexText[i * 3 + 12] = cszHexChar[nByte & 0xf];
      szHexText[61 + i] = (nByte >= 32 && nByte < 127 ? nByte : '.');
    }
  }
  if (nBegin < 8 && nEnd > 8) {
    szHexText[34] = '-';
  }
  return 79;
}

void DumpHex(char *szHexText, const char *lpBuffer, int nOffset, int nLength) {
  int i, nOffsetDiv16, nEnd, nEndDiv16, nTextLen;

  nEnd = nOffset + nLength;
  nOffsetDiv16 = nOffset / 16;
  nEndDiv16 = nEnd / 16;
  if (nOffsetDiv16 == nEndDiv16) {
    nTextLen = DumpLine(szHexText, lpBuffer, nOffsetDiv16, nOffset % 16, nEnd % 16);
  } else {
    nTextLen = DumpLine(szHexText, lpBuffer, nOffsetDiv16, nOffset % 16, 16);
    for (i = nOffsetDiv16 + 1; i < nEndDiv16; i ++) {
      nTextLen += DumpLine(szHexText + nTextLen, lpBuffer, i, 0, 16);
    }
    if (nEnd % 16 > 0) {
      nTextLen += DumpLine(szHexText + nTextLen, lpBuffer, nEndDiv16, 0, nEnd % 16); 
    }
  }
  szHexText[nTextLen] = '\0';
}