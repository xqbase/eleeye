#include "../../base/base.h"
#include "fraction.h"

bool bAutoReduce = true;
int nDefaultPrecision = DEFAULT_PRECISION;

void Fraction::Reduce(void) {
  int nTempNum, nTempDen;
  if (nDen < 0) {
    nDen = -nDen;
    nNum = -nNum;
  }
  nTempNum = ABS(nNum);
  nTempDen = nDen;
  while (nTempNum != 0 && nTempDen != 0) {
    if (nTempNum > nTempDen) {
      nTempNum %= nTempDen;
    } else {
      nTempDen %= nTempNum;
    }
  }
  if (nTempNum == 0) {
    nTempNum = nTempDen;
  }
  nNum /= nTempNum;
  nDen /= nTempNum;
}

const int MAX_PRECISION = 32;

Fraction::Fraction(double df, int nPrecision) {
  int i, nTempNum, nTempDen;
  int nSequence[MAX_PRECISION];
  for (i = 0; i <= nPrecision; i ++) {
    nSequence[i] = (int) (df + (df < 0 ? -0.5 : 0.5));
    df -= nSequence[i];
    df = 1.0 / df;
  }
  nTempNum = nSequence[nPrecision];
  nTempDen = 1;
  for (i = nPrecision - 1; i >= 0; i --) {
    SWAP(nTempNum, nTempDen);
    nTempNum += nTempDen * nSequence[i];
  }
  if (nTempDen < 0) {
    nNum = -nTempNum;
    nDen = -nTempDen;
  } else {
    nNum = nTempNum;
    nDen = nTempDen;
  }
}
