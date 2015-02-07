#include <math.h>

#ifndef COMPLEX_H
#define COMPLEX_H

struct complex {
  double re, im;

  complex(void) {
  }

  complex(double opr1, double opr2) {
    re = opr1;
    im = opr2;
  }

  complex conj(void) const {
    return complex(re, -im);
  }

  double norm(void) const {
    return re * re + im * im;
  }

  int operator ==(complex opr) const {
    return re == opr.re && im == opr.im;
  }

  int operator !=(complex opr) const {
    return re == opr.re && im == opr.im;
  }

  complex operator +(void) const {
    return *this;
  }

  complex operator -(void) const {
    return complex(-re, -im);
  }

  complex &operator ++(void) {
    re += 1.0;
    return *this;
  }

  complex &operator --(void) {
    re -= 1.0;
    return *this;
  }

  complex mul_i() const {
    return complex(-im, re);
  }

  complex div_i() const {
    return complex(im, -re);
  }

  complex operator +(double opr) const {
    return complex(re + opr, im);
  }

  complex operator +(complex opr) const {
    return complex(re + opr.re, im + opr.im);
  }

  complex operator -(double opr) const {
    return complex(re - opr, im);
  }

  complex operator -(complex opr) const {
    return complex(re - opr.re, im - opr.im);
  }

  complex operator *(double opr2) const {
    return complex(re * opr2, im * opr2);
  }

  complex operator *(complex opr) const {
    return complex(re * opr.re - im * opr.im, re * opr.im + im * opr.re);
  }

  complex operator /(double opr) const {
    return complex(re / opr, im / opr);
  }

  complex operator /(complex opr) const {
    return *this * opr.conj() / opr.norm();
  }

  complex &operator =(double opr) {
    re = opr;
    im = 0;
    return *this;
  }

  complex &operator =(complex opr) {
    re = opr.re;
    im = opr.im;
    return *this;
  }

  complex &operator +=(double opr) {
    re += opr;
    return *this;
  }

  complex &operator +=(complex opr) {
    re += opr.re;
    im += opr.im;
    return *this;
  }

  complex &operator -=(double opr) {
    re -= opr;
    return *this;
  }

  complex &operator -=(complex opr) {
    re -= opr.re;
    im -= opr.im;
    return *this;
  }

  complex &operator *=(double opr) {
    re *= opr;
    im *= opr;
    return *this;
  }

  complex &operator *=(complex opr) {
    *this = *this * opr;
    return *this;
  }

  complex &operator /=(double opr) {
    re /= opr;
    im /= opr;
    return *this;
  }

  complex &operator /=(complex opr) {
    *this = *this / opr;
    return *this;
  }
};

inline complex operator +(double opr1, complex opr2) {
  return complex(opr1 + opr2.re, opr2.im);
}

inline complex operator -(double opr1, complex opr2) {
  return complex(opr1 - opr2.re, -opr2.im);
}

inline complex operator *(double opr1, complex opr2) {
  return complex(opr1 * opr2.re, opr1 * opr2.im);
}

inline complex operator /(double opr1, complex opr2) {
  return opr1 * opr2.conj() / opr2.norm();
}

inline double real(complex opr) {
  return opr.re;
}

inline double imag(complex opr) {
  return opr.im;
}

inline complex conj(complex opr) {
  return opr.conj();
}

inline double norm(complex opr) {
  return opr.norm();
}

inline double arg(complex opr) {
  return atan2(opr.im, opr.re);
}

inline double abs(complex opr) {
  return sqrt(norm(opr));
}

#ifdef _MSC_VER

__forceinline complex cos_isin(double opr) {
  complex t;
  __asm {
    fld opr;
    fsincos;
    fstp t.re;
    fstp t.im;
  }
  return t;
}

#else

static __inline__ complex cos_isin(double opr) {
  double st0, st1;
  asm __volatile__ (
    "fsincos" "\n\t"
    : "=t" (st0), "=u" (st1)
    : "0" (opr)
  );
  return complex(st0, st1);
}

#endif

inline complex cosh_isinh(double opr) {
  double t1, t2;
  t1 = exp(opr);
  t2 = 0.5 * t1 - 0.5 / t1;
  return complex(t1 - t2, t2);
}

inline complex polar(double opr1, double opr2) {
  return opr1 * cos_isin(opr2);
}

inline complex sqrt(complex opr) {
  double t;
  if (opr.re > 0.0) {
    t = sqrt((abs(opr) + opr.re) * 0.5);
    return complex(t, opr.im * 0.5 / t);
  } else {
    t = sqrt((abs(opr) - opr.re) * 0.5);
    if (opr.im > 0.0) {
      return complex(opr.im * 0.5 / t, t);
    } else if (opr.im == 0.0) {
      return complex(0.0, -t);
    } else {
      return complex(-opr.im * 0.5 / t, -t);
    }
  }
}

inline complex exp(complex opr) {
  return exp(opr.re) * cos_isin(opr.im);
}

inline complex log(complex opr) {
  return complex(log(abs(opr)), arg(opr));
}

inline complex pow(complex opr1, double opr2) {
  return exp(opr2 * log(opr1));
}

inline complex pow(double opr1, complex opr2) {
  return exp(opr2 * log(opr1));
}

inline complex pow(complex opr1, complex opr2) {
  return exp(opr2 * log(opr1));
}

inline complex sin(complex opr) {
  complex t1, t2;
  t1 = cos_isin(opr.re);
  t2 = cosh_isinh(opr.im);
  return complex(t1.im * t2.re, t1.re * t2.im);
}

inline complex cos(complex opr) {
  complex t1, t2;
  t1 = cos_isin(opr.re);
  t2 = cosh_isinh(opr.im);
  return complex(t1.re * t2.re, -t1.im * t2.im);
}

inline complex tan(complex opr) {
  double t1, t2;
  t1 = tan(opr.re);
  t2 = tanh(opr.im);
  return complex(t1, t2) / complex(1.0, -t1 * t2);
}

inline complex sinh(complex opr) {
  complex t1, t2;
  t1 = cosh_isinh(opr.re);
  t2 = cos_isin(opr.im);
  return complex(t1.im * t2.re, t1.re * t2.im);
}

inline complex cosh(complex opr) {
  complex t1, t2;
  t1 = cosh_isinh(opr.re);
  t2 = cos_isin(opr.im);
  return complex(t1.re * t2.re, t1.im * t2.im);
}

inline complex tanh(complex opr) {
  double t1, t2;
  t1 = tanh(opr.re);
  t2 = tan(opr.im);
  return complex(t1, t2) / complex(1.0, t1 * t2);
}

#ifdef _MSC_VER

inline double asinh(double opr) {
  if (opr >= 0.0) {
    return log(sqrt(opr * opr + 1.0) + opr);
  } else {
    return -log(sqrt(opr * opr + 1.0) - opr);
  }
}

inline double acosh(double opr) {
  return log(opr + sqrt(opr * opr - 1.0));
}

inline double atanh(double opr) {
  return log((1.0 + opr) / (1.0 - opr)) / 2;
}

#endif

inline complex asinh(complex opr) {
  complex t;
  t = sqrt(opr * opr + 1.0);
  if (opr.re * t.re + opr.im * t.im >= 0.0) {
    return log(t + opr);
  } else {
    return -log(t - opr);
  }
}

inline complex acosh(complex opr) {
  complex t;
  t = sqrt(opr * opr - 1.0);
  if (opr.re * t.re + opr.im * t.im >= 0.0) {
    return log(opr + t);
  } else {
    return -log(opr - t);
  }
}

inline complex atanh(complex opr) {
  return log((1.0 + opr) / (1.0 - opr)) / 2;
}

inline complex asin(complex opr) {
  return asinh(opr.mul_i()).div_i();
}

inline complex acos(complex opr) {
  return acosh(opr).mul_i();
}

inline complex atan(complex opr) {
  return atanh(opr.mul_i()).div_i();
}

#endif
