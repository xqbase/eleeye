#ifndef BASE64_H
#define BASE64_H

void B64Enc(char *szAsc, void *lpBlock, int nBlockLen, int nLineLen);
int B64Dec(void *lpBlock, const char *szAsc);

#endif
