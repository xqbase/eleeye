#ifndef FRACTION_H
#define FRACTION_H

const bool SKIP_REDUCE = true;
const int DEFAULT_PRECISION = 4;

extern bool bAutoReduce;
extern int nDefaultPrecision;

struct Fraction {
  int nNum, nDen;

  void Reduce(void);
  Fraction(void) {
    // Do Nothing
  }
  Fraction(int n1, int n2 = 1, bool bSkipReduce = false) {
    nNum = n1;
    nDen = n2;
    if (bAutoReduce && !bSkipReduce) {
      Reduce();
    }
  }
  Fraction(double df, int nPrecision = nDefaultPrecision);
  Fraction operator +(void) const {
    return *this;
  }
  operator int(void) const {
    return nNum / nDen;
  }
  operator double(void) const {
    return (double) nNum / nDen;
  }
  Fraction operator -(void) const {
    return Fraction(-nNum, nDen, SKIP_REDUCE);
  }
  int operator <(Fraction fract) const {
    return nDen * fract.nDen < 0 ? nNum * fract.nDen > fract.nNum * nDen : nNum * fract.nDen < fract.nNum * nDen;
  }
  int operator <=(Fraction fract) const {
    return nDen * fract.nDen < 0 ? nNum * fract.nDen >= fract.nNum * nDen : nNum * fract.nDen <= fract.nNum * nDen;
  }
  int operator >(Fraction fract) const {
    return nDen * fract.nDen < 0 ? nNum * fract.nDen < fract.nNum * nDen : nNum * fract.nDen > fract.nNum * nDen;
  }
  int operator >=(Fraction fract) const {
    return nDen * fract.nDen < 0 ? nNum * fract.nDen <= fract.nNum * nDen : nNum * fract.nDen >= fract.nNum * nDen;
  }
  int operator ==(Fraction fract) const {
    return nNum * fract.nDen == fract.nNum * nDen;
  }
  int operator !=(Fraction fract) const {
    return nNum * fract.nDen != fract.nNum * nDen;
  }
  int operator <(int n) const {
    return nDen < 0 ? nNum > n * nDen : nNum < n * nDen;
  }
  int operator <=(int n) const {
    return nDen < 0 ? nNum >= n * nDen : nNum <= n * nDen;
  }
  int operator >(int n) const {
    return nDen < 0 ? nNum < n * nDen : nNum > n * nDen;
  }
  int operator >=(int n) const {    
    return nDen < 0 ? nNum <= n * nDen : nNum >= n * nDen;
  }
  int operator ==(int n) const {
    return nNum == n * nDen;
  }
  int operator !=(int n) const {
    return nNum != n * nDen;
  }
  Fraction operator +(Fraction fract) const {
    return Fraction(nNum * fract.nDen + fract.nNum * nDen, nDen * fract.nDen);
  }
  Fraction operator -(Fraction fract) const {
    return Fraction(nNum * fract.nDen - fract.nNum * nDen, nDen * fract.nDen);
  }
  Fraction operator *(Fraction fract) const {
    return Fraction(nNum * fract.nNum, nDen * fract.nDen);
  }
  Fraction operator /(Fraction fract) const {
    return Fraction(nNum * fract.nDen, fract.nNum * nDen);
  }
  Fraction operator +(int n) const {
    return Fraction(nNum + n * nDen, nDen, SKIP_REDUCE);
  }
  Fraction operator -(int n) const {
    return Fraction(nNum - n * nDen, nDen, SKIP_REDUCE);
  }
  Fraction operator *(int n) const {
    return Fraction(nNum * n, nDen);
  }
  Fraction operator /(int n) const {
    return Fraction(nNum, nDen * n);
  }
  Fraction &operator +=(Fraction fract) {
    nNum *= fract.nDen;
    nNum += fract.nNum * nDen;
    nDen *= fract.nDen;
    if (bAutoReduce) {
      Reduce();
    }
    return *this;
  }
  Fraction &operator -=(Fraction fract) {
    nNum *= fract.nDen;
    nNum -= fract.nNum * nDen;
    nDen *= fract.nDen;
    if (bAutoReduce) {
      Reduce();
    }
    return *this;
  }
  Fraction &operator *=(Fraction fract) {
    nNum *= fract.nNum;
    nDen *= fract.nDen;
    if (bAutoReduce) {
      Reduce();
    }
    return *this;
  }
  Fraction &operator /=(Fraction fract) {
    nNum *= fract.nDen;
    nDen *= fract.nNum;
    if (bAutoReduce) {
      Reduce();
    }
    return *this;
  }
  Fraction &operator +=(int n) {
    nNum += n * nDen;
    return *this;
  }
  Fraction &operator -=(int n) {
    nNum -= n * nDen;
    return *this;
  }
  Fraction &operator *=(int n) {
    nNum *= n;
    if (bAutoReduce) {
      Reduce();
    }
    return *this;
  }
  Fraction &operator /=(int n) {
    nDen *= n;
    if (bAutoReduce) {
      Reduce();
    }
    return *this;
  }
  Fraction operator ++(void) {
    nNum += nDen;
    return *this;
  }
  Fraction operator --(void) {
    nNum -= nDen;
    return *this;
  }
}; // fract

inline int operator <(int n, Fraction fract) {
  return fract.nDen < 0 ? n * fract.nDen > fract.nNum : n * fract.nDen < fract.nNum;
}

inline int operator <=(int n, Fraction fract) {
  return fract.nDen < 0 ? n * fract.nDen >= fract.nNum : n * fract.nDen <= fract.nNum;
}

inline int operator >(int n, Fraction fract) {
  return fract.nDen < 0 ? n * fract.nDen < fract.nNum : n * fract.nDen > fract.nNum;
}

inline int operator >=(int n, Fraction fract) {
  return fract.nDen < 0 ? n * fract.nDen <= fract.nNum : n * fract.nDen >= fract.nNum;
}

inline int operator ==(int n, Fraction fract) {
  return n * fract.nDen == fract.nNum;
}

inline int operator !=(int n, Fraction fract) {
  return n * fract.nDen != fract.nNum;
}

inline Fraction operator +(int n, Fraction fract) {
  return Fraction(n * fract.nDen + fract.nNum, fract.nDen, SKIP_REDUCE);
}

inline Fraction operator -(int n, Fraction fract) {
  return Fraction(n * fract.nDen - fract.nNum, fract.nDen, SKIP_REDUCE);
}

inline Fraction operator *(int n, Fraction fract) {
  return Fraction(n * fract.nNum, fract.nDen);
}

inline Fraction operator /(int n, Fraction fract) {
  return Fraction(n * fract.nDen, fract.nNum);
}

#endif
