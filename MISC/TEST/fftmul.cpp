#include <stdio.h>
#include <time.h>
#include "../../base/base.h"
#include "../../base/rc4prng.h"
#include "bigint32.h"
#include "complex.h"
#include "fft.h"

struct FftBigIntMul {
  RealFft rf;
  InvRealFft irf;
  double *dfDst, *dfSrc1, *dfSrc2;
  complex *cDst, *cSrc1, *cSrc2;

  FftBigIntMul(void) {
    // Do Nothing
  }

  FftBigIntMul(int nLenParam) {
    Init(nLenParam);
  }

  ~FftBigIntMul(void) {
    delete[] dfDst;
    delete[] dfSrc1;
    delete[] dfSrc2;
    delete[] cDst;
    delete[] cSrc1;
    delete[] cSrc2;
  }

  void Init(int nLenParam) {
    int nLen = nLenParam * 4;
    rf.Init(nLen);
    irf.Init(nLen);
    dfDst = new double[nLen];
    dfSrc1 = new double[nLen];
    dfSrc2 = new double[nLen];
    cDst = new complex[nLen];
    cSrc1 = new complex[nLen];
    cSrc2 = new complex[nLen];
  }

  void Exec(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2);
};

void FftBigIntMul::Exec(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2) {
  int i, dw, nLen, nLen2;
  uint16_t *lpwDst;
  const uint16_t *lpcwSrc1, *lpcwSrc2;

  nLen = rf.nLen;
  nLen2 = nLen / 2;
  lpwDst = (uint16_t *) lpDst;
  lpcwSrc1 = (const uint16_t *) lpSrc1;
  lpcwSrc2 = (const uint16_t *) lpSrc2;
  for (i = 0; i < nLen2; i ++) {
    dfSrc1[i] = lpcwSrc1[i];
    dfSrc1[nLen2 + i] = 0.0;
    dfSrc2[i] = lpcwSrc2[i];
    dfSrc2[nLen2 + i] = 0.0;
  }
  rf.Exec(cSrc1, dfSrc1);
  rf.Exec(cSrc2, dfSrc2);
  for (i = 0; i < nLen; i ++) {
    cDst[i] = cSrc1[i] * cSrc2[i];
  }
  irf.Exec(dfDst, cDst);
  dw = 0;
  for (i = 0; i < nLen; i ++) {
    dfDst[i] += dw;
    dw = (uint32_t) ((dfDst[i] + 0.5) / 65536.0);
    lpwDst[i] = (uint16_t) (dfDst[i] - 65536.0 * dw + 0.5);
  }
}

const int TEST_TIMES = 256;

int main(void) {
  RC4Struct rc4;
  int64_t llTime;
  int i, nArrLen;
  uint32_t *lpDst1, *lpDst2, *lpSrc1, *lpSrc2;

  rc4.InitZero();
  nArrLen = 32;
  printf("Multiplication Test (in microseconds)\n");
  printf("\n");
  printf(" Bits Normal   FFT\n");
  printf("==================\n");
  while (nArrLen <= 2048) {
    lpDst1 = new uint32_t[nArrLen * 2];
    lpDst2 = new uint32_t[nArrLen * 2];
    lpSrc1 = new uint32_t[nArrLen];
    lpSrc2 = new uint32_t[nArrLen];
    FftBigIntMul fbim(nArrLen);
    for (i = 0; i < nArrLen; i ++) {
      lpSrc1[i] = rc4.NextLong();
      lpSrc2[i] = rc4.NextLong();
    }
    llTime = GetTime();
    for (i = 0; i < TEST_TIMES; i ++) {
      BigIntMul(lpDst1, lpSrc1, lpSrc2, nArrLen, nArrLen);
    }
    printf("%5d  %5d", nArrLen * 32, (int) (GetTime() - llTime) * 1000 / TEST_TIMES);
    llTime = GetTime();
    for (i = 0; i < TEST_TIMES; i ++) {
      fbim.Exec(lpDst2, lpSrc1, lpSrc2);
    }
    printf(" %5d\n", (int) (GetTime() - llTime) * 1000 / TEST_TIMES);
    for (i = 0; i < nArrLen * 2; i ++) {
      if (lpDst1[i] != lpDst2[i]) {
        printf("Error at %d: %08X %08X\n", i, lpDst1[i], lpDst2[i]);
      }
    }
    delete[] lpDst1;
    delete[] lpDst2;
    delete[] lpSrc1;
    delete[] lpSrc2;
    nArrLen <<= 1;
  }
  return 0;
};
