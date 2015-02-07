#include <stdio.h>

const char *szTempFile = "UNIX2DOS.TMP";

int main(int argc, char **argv) {
  FILE *fpIn, *fpOut;
  int nChar;

  if (argc < 2) {
    fpIn = stdin;
    fpOut = stdout;
  } else {
    fpIn = fopen(argv[1], "rb");
    if (fpIn == NULL) {
      return 1;
    }
    if (argc < 3) {
      fpOut = fopen(szTempFile, "wb");
    } else {
      fpOut = fopen(argv[2], "wb");
    }
    if (fpOut == NULL) {
      fclose(fpIn);
      return 1;
    }
  }

  while ((nChar = fgetc(fpIn)) != EOF) {
    if (nChar == '\n') {
      fputc('\r', fpOut);
    }
    fputc(nChar, fpOut);
  }

  if (argc > 1) {
    fclose(fpIn);
    fclose(fpOut);
    if (argc < 3) {
      remove(argv[1]);
      rename(szTempFile, argv[1]);
    }
  }
  return 0;
}
