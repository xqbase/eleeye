#include <stdio.h>

const char *const cszEncTab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int main(int argc, char **argv) {
  long i, nFileLen, nBlocks;
  unsigned char ucBlock[4];
  FILE *fp;
  if (argc == 1) {
    printf("=== Base64 Encoding Program ===\n");
    printf("Usage: B64ENC Binary-File > Base64-File\n");
  } else {
    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
      printf("Unable to Open File: %s\n", argv[1]);
    } else {
      fseek(fp, 0, SEEK_END);
      nFileLen = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      nBlocks = nFileLen / 3;
      for (i = 0; i < nBlocks; i ++) {
        fread(ucBlock, 3, 1, fp);
        putchar(cszEncTab[ucBlock[0] >> 2]);
        putchar(cszEncTab[((ucBlock[0] & 3) << 4) + (ucBlock[1] >> 4)]);
        putchar(cszEncTab[((ucBlock[1] & 15) << 2) + (ucBlock[2] >> 6)]);
        putchar(cszEncTab[ucBlock[2] & 63]);
        if ((i + 1) % 19 == 0) {
          printf("\n");
        }
      }
      if (nFileLen % 3 > 0) {
        fread(ucBlock, 3, 1, fp);
        putchar(cszEncTab[ucBlock[0] >> 2]);
        if (nFileLen % 3 > 1) {
          putchar(cszEncTab[((ucBlock[0] & 3) << 4) + (ucBlock[1] >> 4)]);
          putchar(cszEncTab[(ucBlock[1] & 15) << 2]);
        } else {
          putchar(cszEncTab[(ucBlock[0] & 3) << 4]);
          putchar('=');
        }
        putchar('=');
      }
      fclose(fp);
    }
  }
  return 0;
}
