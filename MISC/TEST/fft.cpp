#include <stdlib.h>
#include "complex.h"
#include "fft.h"

static const double PI = 3.141592653589793;

void MakExp(complex *lpExp, int nLen) {
  int i, iMax, jMax;
  double t;
  complex z;
  lpExp[0] = complex(1.0, 0.0);
  iMax = 1;
  if (nLen < 0) {
    jMax = -(nLen / 4);
    t = PI / 2;
  } else {
    jMax = nLen / 4;
    t = -PI / 2;
  }
  while (jMax != 0) {
    z = cos_isin(t);
    for (i = 0; i < iMax; i ++) {
      lpExp[(i * 2 + 1) * jMax] = lpExp[i * 2 * jMax] * z;
    }
    iMax *= 2;
    jMax /= 2;
    t /= 2;
  }
}

// base-4 Cooley-Tukey FFT algorithm
void Fft(complex *lpDst, const complex *lpSrc, const complex *lpExp, int nLen) {
  int i, j, k, l;
  complex a, b, c, d, z;
  if (nLen == 1) {
    lpDst[0] = lpSrc[0];
  } else if (nLen == 2) {
    lpDst[0] = lpSrc[0] + lpSrc[1];
    lpDst[1] = lpSrc[0] - lpSrc[1];
  } else {
    j = 0;
    for (i = 0; i < nLen; i ++) {
      lpDst[i] = lpSrc[j];
      k = nLen >> 1;
      while (j >= k && k > 0) {
        j -= k;
        k >>= 1;
      }
      j += k;
    }
    for (i = 0; i < nLen; i += 4) {
      a = lpDst[i] + lpDst[i + 1];
      b = lpDst[i + 2] + lpDst[i + 3];
      c = lpDst[i] - lpDst[i + 1];
      d = (lpDst[i + 2] - lpDst[i + 3]) * lpExp[nLen / 4];
      lpDst[i] = a + b;
      lpDst[i + 1] = c + d;
      lpDst[i + 2] = a - b;
      lpDst[i + 3] = c - d;
    }
    for (i = 4; i < nLen; i <<= 1) {
      l = nLen / (i << 1);
      for (j = 0; j < nLen; j += i << 1) {
        for (k = 0; k < i; k ++) {
          z = lpDst[i + j + k] * lpExp[k * l];
          lpDst[i + j + k] = lpDst[j + k] - z;
          lpDst[j + k] += z;
        }
      }
    }
  }
}

void RealFft::Exec(complex *lpDst, const double *lpSrc) {
  int i, nLen2;
  complex z;
  if (nLen == 1) {
    lpDst[0] = complex(lpSrc[0], 0);
  } else {
    nLen2 = nLen / 2;
    for (i = 0; i < nLen2; i ++) {
      lpDst[i] = complex(lpSrc[i * 2], lpSrc[i * 2 + 1]);
    }
    Fft(lpTemp, lpDst, lpExp2, nLen2);
    lpDst[0] = complex((lpTemp[0].re + lpTemp[0].im) * 2, 0);
    lpDst[nLen2] = complex((lpTemp[0].re - lpTemp[0].im) * 2, 0);
    for (i = 1; i < nLen2; i ++) {
      z = lpTemp[nLen2 - i].conj();
      lpDst[i] = lpTemp[i] + z + (lpTemp[i] - z) * lpExp[i].div_i();
      lpDst[nLen - i] = lpDst[i].conj();
    }
    for (i = 0; i < nLen; i ++) {
      lpDst[i] *= 0.5;
    }
  }
}

void InvRealFft::Exec(double *lpDst, const complex *lpSrc) {
  int i, nLen;
  nLen = rf.nLen;
  for (i = 0; i < nLen; i ++) {
    lpDst[i] = lpSrc[i].re + lpSrc[i].im;
  }
  rf.Exec(lpTemp2, lpDst);
  for (i = 0; i < nLen; i ++) {
    lpDst[i] = (lpTemp2[i].re - lpTemp2[i].im) / nLen;
  }
}
