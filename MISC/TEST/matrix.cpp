#include "../../base/base.h"
#include "matrix.h"

Matrix Matrix::DelRowCol(int nRowStart, int nRowLen, int nColStart, int nColLen) const {
  Matrix matrRet(nRow - nRowLen, nCol - nColLen);
  matrRet.Load(0, 0, *this, 0, 0, nRowStart, nColStart);
  matrRet.Load(nRowStart, 0, *this, nRowStart + nRowLen, 0, nRow - nRowStart - nRowLen, nColStart);
  matrRet.Load(0, nColStart, *this, 0, nColStart + nColLen, nRowStart, nCol - nColStart - nColLen);
  matrRet.Load(nRowStart, nColStart, *this, nRowStart + nRowLen, nColStart + nColLen,
      nRow - nRowStart - nRowLen, nCol - nColStart - nColLen);
  return matrRet;
}

Matrix Matrix::DelRow(int nStart, int nLen) const {
  Matrix matrRet(nRow - nLen, nCol);
  matrRet.Load(0, 0, *this, 0, 0, nStart, nCol);
  matrRet.Load(nStart, 0, *this, nStart + nLen, 0, nRow - nStart - nLen, nCol);
  return matrRet;
}

Matrix Matrix::DelCol(int nStart, int nLen) const {
  Matrix matrRet(nRow, nCol - nLen);
  matrRet.Load(0, 0, *this, 0, 0, nRow, nStart);
  matrRet.Load(0, nStart, *this, 0, nStart + nLen, nRow, nCol - nStart - nLen);
  return matrRet;
}

Matrix Matrix::operator -(void) const {
  int i, j;
  Matrix matrRet(nRow, nCol);
  for (i = 0; i < nRow; i ++) {
    for (j = 0; j < nCol; j ++) {
      matrRet[i][j] = -(*this)[i][j];
    }
  }
  return matrRet;
}

Matrix Matrix::operator +(const Matrix &matr) const {
  int i, j;
  Matrix matrRet(nRow, nCol);
  for (i = 0; i < nRow; i ++) {
    for (j = 0; j < nCol; j ++) {
      matrRet[i][j] = (*this)[i][j] + matr[i][j];
    }
  }
  return matrRet;
}

Matrix Matrix::operator -(const Matrix &matr) const {
  int i, j;
  Matrix matrRet(nRow, nCol);
  for (i = 0; i < nRow; i ++) {
    for (j = 0; j < nCol; j ++) {
      matrRet[i][j] = (*this)[i][j] - matr[i][j];
    }
  }
  return matrRet;
}

Matrix Matrix::operator *(double dfReal) const {
  int i, j;
  Matrix matrRet(nRow, nCol);
  for (i = 0; i < nRow; i ++) {
    for (j = 0; j < nCol; j ++) {
      matrRet[i][j] = (*this)[i][j] * dfReal;
    }
  }
  return matrRet;
}

Matrix Matrix::operator /(double dfReal) const {
  int i, j;
  Matrix matrRet(nRow, nCol);
  for (i = 0; i < nRow; i ++) {
    for (j = 0; j < nCol; j ++) {
      matrRet[i][j] = (*this)[i][j] / dfReal;
    }
  }
  return matrRet;
}

Matrix Matrix::operator *(const Matrix &matr) const {
  int i, j, k;
  Matrix matrRet(nRow, matr.nCol);
  for (i = 0; i < nRow; i ++) {
    for (j = 0; j < matr.nCol; j ++) {
      for (k = 0; k < nCol; k ++) {
        matrRet[i][j] += (*this)[i][k] * matr[k][j];
      }
    }
  }
  return matrRet;
}

Matrix Matrix::Trans(void) const {
  int i, j;
  Matrix matrRet(nCol, nRow);
  for (i = 0; i < nCol; i ++) {
    for (j = 0; j < nRow; j ++) {
      matrRet[i][j] = (*this)[j][i];
    }
  }
  return matrRet;
}

double Matrix::Det(void) const {
  int i, j, jMax;
  double dfRet, dfThis, dfMax;
  Matrix matrTmp(*this);
  dfRet = 1.0;
  for (i = 0; i < nRow; i ++) {
    // Step 1:
    dfMax = 0.0;
    jMax = i;
    for (j = i; j < nRow; j ++) {
      dfThis = ABS(matrTmp[i][j]);
      if (dfThis > dfMax) {
        jMax = j;
        dfMax = dfThis;
      }
    }
    if (dfMax == 0.0) {
      return 0.0;
    }
    if (jMax != i) {
      dfRet = -dfRet;
      matrTmp.ColSwap(i, jMax);
    }
    // Step 2:
    dfRet *= matrTmp[i][i];
    matrTmp.RowMul(i, 1.0 / matrTmp[i][i], i);
    for (j = i + 1; j < nRow; j ++) {
      if (matrTmp[j][i] != 0.0) {
        dfRet *= matrTmp[j][i];
        matrTmp.RowMul(j, 1.0 / matrTmp[j][i], i);
        matrTmp.RowSub(j, i, i);
      }
    }
  }
  return dfRet;
}

Matrix Matrix::LeftDiv(const Matrix &matr) const {
  int i, j, jMax;
  double dfThis, dfMax;
  int *nSwapList;
  Matrix matrRet(*this), matrTmp(matr);
  nSwapList = new int[nRow];
  for (i = 0; i < nRow; i ++) {
    // Step 1:
    dfMax = 0.0;
    jMax = i;
    for (j = i; j < nRow; j ++) {
      dfThis = ABS(matrTmp[i][j]);
      if (dfThis > dfMax) {
        jMax = j;
        dfMax = dfThis;
      }
    }
    if (dfMax == 0.0) {
      delete[] nSwapList;
      return Matrix(nRow, nRow);
    }
    nSwapList[i] = jMax;
    if (jMax != i) {
      matrTmp.ColSwap(i, jMax);
    }
    // Step 2:
    matrRet.RowMul(i, 1.0 / matrTmp[i][i]);
    matrTmp.RowMul(i, 1.0 / matrTmp[i][i], i);
    for (j = i + 1; j < nRow; j ++) {
      if (matrTmp[j][i] != 0.0) {
        matrRet.RowMul(j, 1.0 / matrTmp[j][i]);        
        matrRet.RowSub(j, i);
        matrTmp.RowMul(j, 1.0 / matrTmp[j][i], i);
        matrTmp.RowSub(j, i, i);
      }
    }
  }
  // Step 3:
  for (i = nRow - 1; i >= 0; i --) {
    for (j = i - 1; j >= 0; j --) {
      if (matrTmp[j][i] != 0.0) {
        matrRet.RowSubMul(j, i, matrTmp[j][i]);
      }
    }
  }
  // Step 4:
  for (i = nRow - 1; i >= 0; i --) {
    if (nSwapList[i] != i) {
      matrRet.RowSwap(i, nSwapList[i]);
    }
  }
  delete[] nSwapList;
  return matrRet;
}

Matrix Matrix::RightDiv(const Matrix &matr) const {
  int i, j, jMax;
  double dfThis, dfMax;
  int *nSwapList;
  Matrix matrRet(*this), matrTmp(matr);
  nSwapList = new int[nCol];
  for (i = 0; i < nCol; i ++) {
    // Step 1:
    dfMax = 0.0;
    jMax = i;
    for (j = i; j < nCol; j ++) {
      dfThis = ABS(matrTmp[j][i]);
      if (dfThis > dfMax) {
        jMax = j;
        dfMax = dfThis;
      }
    }
    if (dfMax == 0.0) {
      delete[] nSwapList;
      return Matrix(nCol, nCol);
    }
    nSwapList[i] = jMax;
    if (jMax != i) {
      matrTmp.RowSwap(i, jMax);
    }
    // Step 2:
    matrRet.ColMul(i, 1.0 / matrTmp[i][i]);
    matrTmp.ColMul(i, 1.0 / matrTmp[i][i], i);
    for (j = i + 1; j < nCol; j ++) {
      if (matrTmp[i][j] != 0.0) {
        matrRet.ColMul(j, 1.0 / matrTmp[i][j]);
        matrRet.ColSub(j, i);
        matrTmp.ColMul(j, 1.0 / matrTmp[i][j], i);
        matrTmp.ColSub(j, i, i);
      }
    }
  }
  // Step 3:
  for (i = nCol - 1; i >= 0; i --) {
    for (j = i - 1; j >= 0; j --) {
      if (matrTmp[i][j] != 0.0) {
        matrRet.ColSubMul(j, i, matrTmp[i][j]);
      }
    }
  }
  // Step 4:
  for (i = nCol - 1; i >= 0; i --) {
    if (nSwapList[i] != i) {
      matrRet.ColSwap(i, nSwapList[i]);
    }
  }
  delete[] nSwapList;
  return matrRet;
}

Matrix Matrix::Inv(void) const {
  int i;
  Matrix matrRet(nRow, nRow);
  for (i = 0; i < nRow; i ++) {
    matrRet[i][i] = 1.0;
  }
  return matrRet.LeftDiv(*this);
}

Matrix operator *(double dfReal, const Matrix &matr) {
  int i, j;
  Matrix matrRet(matr.nRow, matr.nCol);
  for (i = 0; i < matr.nRow; i ++) {
    for (j = 0; j < matr.nCol; j ++) {
      matrRet[i][j] = matr[i][j] * dfReal;
    }
  }
  return matrRet;
}

Matrix Merge(const Matrix &matrTopLeft, const Matrix &matrBottomLeft,
    const Matrix &matrTopRight, const Matrix &matrBottomRight) {
  Matrix matrRet(matrTopLeft.nRow + matrBottomLeft.nRow, matrTopLeft.nCol + matrTopRight.nCol);
  matrRet.Load(0, 0, matrTopLeft, 0, 0, matrTopLeft.nRow, matrTopLeft.nCol);
  matrRet.Load(matrTopLeft.nRow, 0, matrBottomLeft, 0, 0, matrBottomLeft.nRow, matrBottomLeft.nCol);
  matrRet.Load(0, matrTopLeft.nCol, matrTopRight, 0, 0, matrTopRight.nRow, matrTopRight.nCol);
  matrRet.Load(matrTopLeft.nRow, matrTopLeft.nCol, matrBottomRight, 0, 0, matrBottomRight.nRow, matrBottomRight.nCol);
  return matrRet;
}

Matrix MergeRow(const Matrix &matrTop, const Matrix &matrBottom) {
  Matrix matrRet(matrTop.nRow + matrBottom.nRow, matrTop.nCol);
  matrRet.Load(0, 0, matrTop, 0, 0, matrTop.nRow, matrTop.nCol);
  matrRet.Load(matrTop.nRow, 0, matrBottom, 0, 0, matrBottom.nRow, matrBottom.nCol);
  return matrRet;
}

Matrix MergeCol(const Matrix &matrLeft, const Matrix &matrRight) {
  Matrix matrRet(matrLeft.nRow, matrLeft.nCol + matrRight.nCol);
  matrRet.Load(0, 0, matrLeft, 0, 0, matrLeft.nRow, matrLeft.nCol);
  matrRet.Load(0, matrLeft.nCol, matrRight, 0, 0, matrRight.nRow, matrRight.nCol);
  return matrRet;
}
