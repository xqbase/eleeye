#include "complex.h"

#ifndef FFT_H
#define FFT_H

void MakExp(complex *lpExp, int nLen);
void Fft(complex *lpDst, const complex *lpSrc, const complex *lpExp, int nLen);

struct RealFft {
  int nLen;
  complex *lpTemp, *lpExp, *lpExp2;

  RealFft(void) {
    // Do Nothing
  }

  RealFft(int nLenParam) {
    Init(nLenParam);
  }

  ~RealFft(void) {
    delete[] lpTemp;
    delete[] lpExp;
    delete[] lpExp2;
  }

  void Init(int nLenParam) {
    nLen = (nLenParam < 0 ? -nLenParam : nLenParam);
    lpTemp = new complex[nLen];
    lpExp = new complex[nLen / 2];
    lpExp2 = new complex[nLen / 4];
    MakExp(lpExp, nLenParam);
    MakExp(lpExp2, nLenParam / 2);
  }

  void Exec(complex *lpDst, const double *lpSrc);
};

struct InvRealFft {
  RealFft rf;
  complex *lpTemp2;

  InvRealFft(void) {
    // Do Nothing
  }

  InvRealFft(int nLenParam) {
    Init(nLenParam);
  }

  ~InvRealFft(void) {
    delete[] lpTemp2;
  }

  void Init(int nLenParam) {
    rf.Init(-nLenParam);
    lpTemp2 = new complex[rf.nLen];
  }

  void Exec(double *lpDst, const complex *lpSrc);
};

#endif
