#ifdef BASE64_DLL

#include <windows.h>
#include "base64.h"

extern "C" __declspec(dllexport) VOID WINAPI Base64Enc(LPSTR szAsc, LPVOID lpBlock, LONG nBlockLen, LONG nLineLen);
extern "C" __declspec(dllexport) LONG WINAPI Base64Dec(LPVOID lpBlock, LPCSTR szAsc);

VOID WINAPI Base64Enc(LPSTR szAsc, LPVOID lpBlock, LONG nBlockLen, LONG nLineLen) {
  B64Enc(szAsc, lpBlock, nBlockLen, nLineLen);
}

LONG WINAPI Base64Dec(LPVOID lpBlock, LPCSTR szAsc) {
  return B64Dec(lpBlock, szAsc);
}

#endif

const char *const cszEncTab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char cszDecTab[256] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
  -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
  -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

void B64Enc(char *szAsc, void *lpBlock, int nBlockLen, int nLineLen) {
  char *lpAsc;
  unsigned char *lpuc;
  int i, nBlocks;
  lpAsc = szAsc;
  lpuc = (unsigned char *) lpBlock;
  nBlocks = nBlockLen / 3;
  for (i = 0; i < nBlocks; i ++) {   
    *lpAsc = cszEncTab[lpuc[0] >> 2];
    lpAsc ++;
    *lpAsc = cszEncTab[((lpuc[0] & 3) << 4) + (lpuc[1] >> 4)];
    lpAsc ++;
    *lpAsc = cszEncTab[((lpuc[1] & 15) << 2) + (lpuc[2] >> 6)];
    lpAsc ++;
    *lpAsc = cszEncTab[lpuc[2] & 63];
    lpAsc ++;
    if (nLineLen > 0) {
      if ((i + 1) % nLineLen == 0) {
        *lpAsc = '\r';
        lpAsc ++;
        *lpAsc = '\n';
        lpAsc ++;
      }
    }
    lpuc += 3;
  }
  if (nBlockLen % 3 > 0) {
    *lpAsc = cszEncTab[lpuc[0] >> 2];
    lpAsc ++;
    if (nBlockLen % 3 > 1) {
      *lpAsc = cszEncTab[((lpuc[0] & 3) << 4) + (lpuc[1] >> 4)];
      lpAsc ++;
      *lpAsc = cszEncTab[(lpuc[1] & 15) << 2];
      lpAsc ++;
    } else {
      *lpAsc = cszEncTab[(lpuc[0] & 3) << 4];
      lpAsc ++;
      *lpAsc = '=';
      lpAsc ++;
    }
    *lpAsc = '=';
    lpAsc ++;
  }
  *lpAsc = '\0';
}

int B64Dec(void *lpBlock, const char *szAsc) {
  int nCounter, nAsc0, nAsc[4];
  const char *lpAsc;
  unsigned char *lpuc;
  lpuc = (unsigned char *) lpBlock;
  nCounter = 0;
  lpAsc = szAsc;
  while (*lpAsc != '\0') {
    nAsc0 = cszDecTab[(int) *lpAsc];
    if (nAsc0 != -1) {
      nAsc[nCounter] = nAsc0;
      nCounter ++;
      if (nCounter == 4) {
        *lpuc = (nAsc[0] << 2) | (nAsc[1] >> 4);
        lpuc ++;
        *lpuc = ((nAsc[1] & 15) << 4) | (nAsc[2] >> 2);
        lpuc ++;
        *lpuc = ((nAsc[2] & 3) << 6) | nAsc[3];
        lpuc ++;
        nCounter = 0;
      }
    }
    lpAsc ++;
  }
  if (nCounter > 1) {
    *lpuc = (nAsc[0] << 2) | (nAsc[1] >> 4);
    lpuc ++;
    if (nCounter > 2) {
      *lpuc = ((nAsc[1] & 15) << 4) | (nAsc[2] >> 2);
      lpuc ++;
    }
  }
  return lpuc - (unsigned char *) lpBlock;
}
