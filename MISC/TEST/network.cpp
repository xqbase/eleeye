#include <stdio.h>
#include "../../base/base.h"
#include "matrix.h"

double Network(int nLength) {
  int nMatLen = nLength * nLength;
  int nInDot, nOutDot, i, j;
  double dfCondSum;
  Matrix mtCurr(nMatLen, 1);
  Matrix mtCond(nMatLen, nMatLen);
  nOutDot = nLength * (nLength + 1) / 2;
  nInDot = nOutDot - nLength - 1;
  mtCurr[nInDot][0] = 1.0;
  mtCurr[nOutDot][0] = -1.0;
  for (i = 0; i < nLength - 1; i ++) {
    for (j = 0; j < nLength - 1; j ++) {
      mtCond[i * nLength + j][(i + 1) * nLength + j] = 1.0;
      mtCond[(i + 1) * nLength + j][i * nLength + j] = 1.0;
      mtCond[i * nLength + j][i * nLength + j + 1] = 1.0;
      mtCond[i * nLength + j + 1][i * nLength + j] = 1.0;
    }
    mtCond[(nLength - 1) * nLength + i][(nLength - 1) * nLength + i + 1] = 1.0;
    mtCond[(nLength - 1) * nLength + i + 1][(nLength - 1) * nLength + i] = 1.0;
    mtCond[i * nLength + nLength - 1][(i + 1) * nLength + nLength - 1] = 1.0;
    mtCond[(i + 1) * nLength + nLength - 1][i * nLength + nLength - 1] = 1.0;
  }
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
  return mtCurr.LeftDiv(mtCond)[nOutDot][0];
}

int main(void) {
  int i;
  int64_t llTime;
  llTime = GetTime();
  for (i = 2; i <= 64; i += 2) {
    printf("R(%d)=%.15f(%dms)\n", i, Network(i), (int) (GetTime() - llTime));
  }
  return 0;
}
