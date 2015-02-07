/*
book.h/book.cpp - Source Code for ElephantEye, Part VI

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.25, Last Modified: Apr. 2011
Copyright (C) 2004-2011 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef BOOK_H
#define BOOK_H

#include <stdio.h>
#include "../base/base.h"
#include "position.h"

struct BookStruct {
  union {
    uint32_t dwZobristLock;
    int nPtr;
  };
  uint16_t wmv, wvl;
}; // bk

inline int BOOK_POS_CMP(const BookStruct &bk, const PositionStruct &pos) {
  return bk.dwZobristLock < pos.zobr.dwLock1 ? -1 :
      bk.dwZobristLock > pos.zobr.dwLock1 ? 1 : 0;
}

struct BookFileStruct {
  FILE *fp;
  int nLen;
  bool Open(const char *szFileName, bool bEdit = false) {
    fp = fopen(szFileName, bEdit ? "r+b" : "rb");
    if (fp == NULL) {
      return false;
    } else {
      fseek(fp, 0, SEEK_END);
      nLen = ftell(fp) / sizeof(BookStruct);
      return true;
    }
  }
  void Close(void) const {
    fclose(fp);
  }
  void Read(BookStruct &bk, int nPtr) const {
    fseek(fp, nPtr * sizeof(BookStruct), SEEK_SET);
    fread(&bk, sizeof(BookStruct), 1, fp);
  }
  void Write(const BookStruct &bk, int nPtr) const {
    fseek(fp, nPtr * sizeof(BookStruct), SEEK_SET);
    fwrite(&bk, sizeof(BookStruct), 1, fp);
  }
};

// 获取开局库着法
int GetBookMoves(const PositionStruct &pos, const char *szBookFile, BookStruct *lpbks);

#endif
