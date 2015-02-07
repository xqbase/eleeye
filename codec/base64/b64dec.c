#include <stdio.h>
#include <string.h>

#define LINE_INPUT_MAX_CHAR 1024

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

int main(int argc, char **argv) {
  int nCounter, nAsc0, nAsc[4];
  FILE *fp;
  unsigned char ucBlock[4];
  char *lpLineStr;
  char szLineStr[LINE_INPUT_MAX_CHAR];
  if (argc == 1) {
    printf("=== Base64 Decoding Program ===\n");
    printf("Usage: B64DEC Binary-File < Base64-File\n");
  } else {
    fp = fopen(argv[1], "wb");
    if (fp == NULL) {
      printf("Unable to Create File: %s\n", argv[1]);
    } else {
      nCounter = 0;
      while (fgets(szLineStr, LINE_INPUT_MAX_CHAR, stdin) != NULL) {
        lpLineStr = szLineStr;
        while (*lpLineStr != '\0') {
          nAsc0 = cszDecTab[(int) *lpLineStr];
          if (nAsc0 != -1) {
            nAsc[nCounter] = nAsc0;
            nCounter ++;
            if (nCounter == 4) {
              ucBlock[0] = (nAsc[0] << 2) | (nAsc[1] >> 4);
              ucBlock[1] = ((nAsc[1] & 15) << 4) | (nAsc[2] >> 2);
              ucBlock[2] = ((nAsc[2] & 3) << 6) | nAsc[3];
              fwrite(ucBlock, 3, 1, fp);
              nCounter = 0;
            }
          }
          lpLineStr ++;
        }
      }
      if (nCounter > 1) {
        ucBlock[0] = (nAsc[0] << 2) | (nAsc[1] >> 4);
        ucBlock[1] = ((nAsc[1] & 15) << 4) | (nAsc[2] >> 2);
        fwrite(ucBlock, nCounter - 1, 1, fp);
      }
      fclose(fp);
    }
  }
  return 0;
}
