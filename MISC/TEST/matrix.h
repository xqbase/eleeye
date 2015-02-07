#include <string.h>
#include "../../base/base.h"

#ifndef MATRIX_H
#define MATRIX_H

struct Matrix {
  int nRow, nCol;
  double *lpdf; // Don't Use It!

  double *operator [](int nIndex) {
    return lpdf + (nIndex * nCol);
  }

  const double *operator [](int nIndex) const {
    return lpdf + (nIndex * nCol);
  }

  Matrix(void) {
    lpdf = NULL;
  }

  Matrix(int nRowLen, int nColLen) {
    nRow = nRowLen;
    nCol = nColLen;
    lpdf = new double[nRow * nCol];
    memset(lpdf, 0, nRow * nCol * sizeof(double));
  }

  Matrix(const Matrix &matr) {
    nRow = matr.nRow;
    nCol = matr.nCol;
    lpdf = new double[nRow * nCol];
    memcpy(lpdf, matr.lpdf, nRow * nCol * sizeof(double));
  }

  ~Matrix(void) {
    if (lpdf != NULL) {
      delete[] lpdf;
      lpdf = NULL;
    }
  }

  Matrix &operator =(const Matrix &matr) {
    if (lpdf != matr.lpdf) {
      if (lpdf != NULL) {
        delete[] lpdf;
        lpdf = NULL;
      }
      nRow = matr.nRow;
      nCol = matr.nCol;
      lpdf = new double[nRow * nCol];
      memcpy(lpdf, matr.lpdf, nRow * nCol * sizeof(double));
    }
    return *this;
  }

  void Load(int nDstRow, int nDstCol, const Matrix &matrSrc, int nSrcRow, int nSrcCol, int nRowLen, int nColLen) {
    int i, j;
    for (i = 0; i < nRowLen; i ++) {
      for (j = 0; j < nColLen; j ++) {
        (*this)[nDstRow + i][nDstCol + j] = matrSrc[nSrcRow + i][nSrcCol + j];
      }
    }
  }

  Matrix Merge(const Matrix &matrBottomLeft, const Matrix &matrTopRight, const Matrix &matrBottomRight) const;
  Matrix MergeRow(const Matrix &matrBottom) const;
  Matrix MergeCol(const Matrix &matrRight) const;
  Matrix DelRowCol(int nRowStart, int nRowLen, int nColStart, int nColLen) const;
  Matrix DelRow(int nStart, int nLen) const;
  Matrix DelCol(int nStart, int nLen) const;

  const Matrix &operator +(void) const {
    return *this;
  }

  Matrix operator -(void) const;
  Matrix operator +(const Matrix &matr) const;
  Matrix operator -(const Matrix &matr) const;
  Matrix operator *(double dfReal) const;
  Matrix operator /(double dfReal) const;

  Matrix &operator +=(const Matrix &matr) {
    int i, j;
    for (i = 0; i < nRow; i ++) {
      for (j = 0; j < nCol; j ++) {
        (*this)[i][j] += matr[i][j];
      }
    }
    return *this;
  }

  Matrix &operator -=(const Matrix &matr) {
    int i, j;
    for (i = 0; i < nRow; i ++) {
      for (j = 0; j < nCol; j ++) {
        (*this)[i][j] -= matr[i][j];
      }
    }
    return *this;
  }

  Matrix &operator *=(double dfReal) {
    int i, j;
    for (i = 0; i < nRow; i ++) {
      for (j = 0; j < nCol; j ++) {
        (*this)[i][j] *= dfReal;
      }
    }
    return *this;
  }

  Matrix &operator /=(double dfReal) {
    int i, j;
    for (i = 0; i < nRow; i ++) {
     for (j = 0; j < nCol; j ++) {
        (*this)[i][j] /= dfReal;
      }
    }
    return *this;
  }

  Matrix operator *(const Matrix &matr) const;

  void RowSwap(int nDstRow, int nSrcRow, int nStart = 0) {
    int i;
    if (nDstRow != nSrcRow) {
      for (i = nStart; i < nCol; i ++) {
        SWAP((*this)[nDstRow][i], (*this)[nSrcRow][i]);
      }
    }
  }

  void RowMul(int nDstRow, double dfReal, int nStart = 0) {
    int i;
    for (i = nStart; i < nCol; i ++) {
      (*this)[nDstRow][i] *= dfReal;
    }
  }

  void RowAdd(int nDstRow, int nSrcRow, int nStart = 0) {
    int i;
    for (i = nStart; i < nCol; i ++) {
      (*this)[nDstRow][i] += (*this)[nSrcRow][i];
    }
  }

  void RowAddMul(int nDstRow, int nSrcRow, double dfReal, int nStart = 0) {
    int i;
    for (i = nStart; i < nCol; i ++) {
      (*this)[nDstRow][i] += (*this)[nSrcRow][i] * dfReal;
    }
  }

  void RowSub(int nDstRow, int nSrcRow, int nStart = 0) {
    int i;
    for (i = nStart; i < nCol; i ++) {
      (*this)[nDstRow][i] -= (*this)[nSrcRow][i];
    }
  }

  void RowSubMul(int nDstRow, int nSrcRow, double dfReal, int nStart = 0) {
    int i;
    for (i = nStart; i < nCol; i ++) {
      (*this)[nDstRow][i] -= (*this)[nSrcRow][i] * dfReal;
    }
  }

  void ColSwap(int nDstCol, int nSrcCol, int nStart = 0) {
    int i;
    if (nDstCol != nSrcCol) {
      for (i = nStart; i < nRow; i ++) {
        SWAP((*this)[i][nDstCol], (*this)[i][nSrcCol]);
      }
    }
  }

  void ColMul(int nDstCol, double dfReal, int nStart = 0) {
    int i;
    for (i = nStart; i < nRow; i ++) {
      (*this)[i][nDstCol] *= dfReal;
    }
  }

  void ColAdd(int nDstCol, int nSrcCol, int nStart = 0) {
    int i;
    for (i = nStart; i < nRow; i ++) {
      (*this)[i][nDstCol] += (*this)[i][nSrcCol];
    }
  }

  void ColAddMul(int nDstCol, int nSrcCol, double dfReal, int nStart = 0) {
    int i;
    for (i = nStart; i < nRow; i ++) {
      (*this)[i][nDstCol] += (*this)[i][nSrcCol] * dfReal;
    }
  }

  void ColSub(int nDstCol, int nSrcCol, int nStart = 0) {
    int i;
    for (i = nStart; i < nRow; i ++) {
      (*this)[i][nDstCol] -= (*this)[i][nSrcCol];
    }
  }

  void ColSubMul(int nDstCol, int nSrcCol, double dfReal, int nStart = 0) {
    int i;
    for (i = nStart; i < nRow; i ++) {
      (*this)[i][nDstCol] -= (*this)[i][nSrcCol] * dfReal;
    }
  }

  Matrix Trans(void) const;
  double Det(void) const;
  Matrix LeftDiv(const Matrix &matr) const;
  Matrix RightDiv(const Matrix &matr) const;
  Matrix Inv(void) const;
}; // matr

inline Matrix Trans(const Matrix &matr) {
  return matr.Trans();
}

inline double Det(const Matrix &matr) {
  return matr.Det();
}

inline Matrix Inv(const Matrix &matr) {
  return matr.Inv();
}

Matrix operator *(double dfReal, const Matrix &matr);
Matrix Merge(const Matrix &matrTopLeft, const Matrix &matrBottomLeft,
    const Matrix &matrTopRight, const Matrix &matrBottomRight);
Matrix MergeRow(const Matrix &matrTop, const Matrix &matrBottom);
Matrix MergeCol(const Matrix &matrLeft, const Matrix &matrRight);

#endif
