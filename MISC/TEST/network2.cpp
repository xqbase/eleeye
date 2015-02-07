#include <stdio.h>
#include "matrix.h"

double Network1(int nHeight, int nWidth) {
  int nMatLen = nWidth * nHeight + 1;
  int nInDot, nOutDot, i, j;
  double dfCondSum;
  Matrix mtCurr(nMatLen, 1);
  Matrix mtCond(nMatLen, nMatLen);
  nOutDot = nWidth * nHeight;
  nInDot = nOutDot - 1;
  mtCurr[nInDot][0] = 1.0;
  mtCurr[nOutDot][0] = -1.0;
  for (i = 0; i < nHeight - 1; i ++) {
    for (j = 0; j < nWidth - 1; j ++) {
      mtCond[i * nWidth + j][(i + 1) * nWidth + j] = 2.0;
      mtCond[(i + 1) * nWidth + j][i * nWidth + j] = 2.0;
      mtCond[i * nWidth + j][i * nWidth + j + 1] = 2.0;
      mtCond[i * nWidth + j + 1][i * nWidth + j] = 2.0;
    }
    mtCond[i * nWidth + nWidth - 1][(i + 1) * nWidth + nWidth - 1] = 2.0;
    mtCond[(i + 1) * nWidth + nWidth - 1][i * nWidth + nWidth - 1] = 2.0;
    mtCond[i * nWidth + nWidth - 1][nOutDot] = 4.0;
    mtCond[nOutDot][i * nWidth + nWidth - 1] = 4.0;
  }
  for (j = 0; j < nWidth - 1; j ++) {
    mtCond[(nHeight - 1) * nWidth + j][(nHeight - 1) * nWidth + j + 1] = 1.0;
    mtCond[(nHeight - 1) * nWidth + j + 1][(nHeight - 1) * nWidth + j] = 1.0;
  }
  mtCond[nInDot][nOutDot] = 2.0;
  mtCond[nOutDot][nInDot] = 2.0;
  for (i = 0; i < nMatLen; i ++) {
    dfCondSum = 0.0;
    for (j = 0; j < nMatLen; j ++) {
       dfCondSum -= mtCond[i][j];
    }
    mtCond[i][i] = dfCondSum;
  }
  for (i = 0; i < nMatLen; i ++) {
    mtCond[nInDot][i] = 0.0;
    mtCond[i][nInDot] = 0.0;
  }
  mtCond[nInDot][nInDot] = 1.0;
  return mtCurr.LeftDiv(mtCond)[nOutDot][0] * 2;
}

double Network2(int nHeight, int nWidth) {
  int nMatLen = nWidth * nHeight + 1;
  int nInDot, nOutDot, i, j;
  double dfCondSum;
  Matrix mtCurr(nMatLen, 1);
  Matrix mtCond(nMatLen, nMatLen);
  nOutDot = nWidth * nHeight;
  nInDot = nOutDot - 1;
  mtCurr[nInDot][0] = 1.0;
  mtCurr[nOutDot][0] = -1.0;
  for (i = 0; i < nHeight - 1; i ++) {
    for (j = 0; j < nWidth - 1; j ++) {
      mtCond[i * nWidth + j][(i + 1) * nWidth + j] = 1.0;
      mtCond[(i + 1) * nWidth + j][i * nWidth + j] = 1.0;
      mtCond[i * nWidth + j][i * nWidth + j + 1] = 1.0;
      mtCond[i * nWidth + j + 1][i * nWidth + j] = 1.0;
      mtCond[i * nWidth + j][(i + 1) * nWidth + j + 1] = 0.5;
      mtCond[(i + 1) * nWidth + j + 1][i * nWidth + j] = 0.5;
      mtCond[i * nWidth + j + 1][(i + 1) * nWidth + j] = 0.5;
      mtCond[(i + 1) * nWidth + j][i * nWidth + j + 1] = 0.5;
    }
    mtCond[i * nWidth + nWidth - 1][(i + 1) * nWidth + nWidth - 1] = 0.5;
    mtCond[(i + 1) * nWidth + nWidth - 1][i * nWidth + nWidth - 1] = 0.5;
    mtCond[i * nWidth + nWidth - 1][nOutDot] = 4.0;
    mtCond[nOutDot][i * nWidth + nWidth - 1] = 4.0;
  }
  for (j = 0; j < nWidth - 1; j ++) {
    mtCond[(nHeight - 1) * nWidth + j][(nHeight - 1) * nWidth + j + 1] = 0.5;
    mtCond[(nHeight - 1) * nWidth + j + 1][(nHeight - 1) * nWidth + j] = 0.5;
  }
  mtCond[nInDot][nOutDot] = 2.0;
  mtCond[nOutDot][nInDot] = 2.0;
  for (i = 0; i < nMatLen; i ++) {
    dfCondSum = 0.0;
    for (j = 0; j < nMatLen; j ++) {
       dfCondSum -= mtCond[i][j];
    }
    mtCond[i][i] = dfCondSum;
  }
  for (i = 0; i < nMatLen; i ++) {
    mtCond[nInDot][i] = 0.0;
    mtCond[i][nInDot] = 0.0;
  }
  mtCond[nInDot][nInDot] = 1.0;
  return mtCurr.LeftDiv(mtCond)[nOutDot][0] * 2;
}

const int ADDIT_HEIGHT = 10;

int main(void) {
  int nHeight, nWidth;
  double dfR1, dfR2, dfR3, dfLastR2, dfLastR3;
  int64_t llTime;
  nWidth = 1;
  nHeight = nWidth + ADDIT_HEIGHT;
  dfLastR2 = Network2(nHeight, nWidth);
  dfLastR3 = dfLastR2 * 0.5 / Network1(nHeight, nWidth);
  for (nWidth ++; nWidth < 70; nWidth ++) {
    nHeight = nWidth + ADDIT_HEIGHT;
    llTime = GetTime();
    dfR1 = Network1(nHeight, nWidth);
    dfR2 = Network2(nHeight, nWidth);
    dfR3 = dfR2 * 0.5 / dfR1;
    dfLastR2 -= dfR2;
    dfLastR3 -= dfR3;
    printf("R=%.15f(%dx%d,%dms)\n", (dfR3 * dfLastR2 - dfR2 * dfLastR3) / (dfLastR2 - dfLastR3), nHeight * 2 + 1, nWidth * 2, (int) (GetTime() - llTime));
    dfLastR2 = dfR2;
    dfLastR3 = dfR3;
  }
  return 0;
}
