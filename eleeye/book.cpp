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

#include "position.h"
#include "book.h"

int GetBookMoves(const PositionStruct &pos, const char *szBookFile, BookStruct *lpbks) {
  BookFileStruct BookFile;
  PositionStruct posScan;
  BookStruct bk;
  int nScan, nLow, nHigh, nPtr;
  int i, j, nMoves;
  // 从开局库中搜索着法的例程，有以下几个步骤：

  // 1. 打开开局库，如果打开失败，则返回空值；
  if (!BookFile.Open(szBookFile)) {
    return 0;
  }

  // 2. 用拆半查找法搜索局面；
  posScan = pos;
  for (nScan = 0; nScan < 2; nScan ++) {
    nPtr = nLow = 0;
    nHigh = BookFile.nLen - 1;
    while (nLow <= nHigh) {
      nPtr = (nLow + nHigh) / 2;
      BookFile.Read(bk, nPtr);
      if (BOOK_POS_CMP(bk, posScan) < 0) {
        nLow = nPtr + 1;          
      } else if (BOOK_POS_CMP(bk, posScan) > 0) {
        nHigh = nPtr - 1;
      } else {
        break;
      }
    }
    if (nLow <= nHigh) {
      break;
    }
    // 原局面和镜像局面各搜索一趟
    posScan.Mirror();
  }

  // 3. 如果不到局面，则返回空着；
  if (nScan == 2) {
    BookFile.Close();
    return 0;
  }
  __ASSERT_BOUND(0, nPtr, BookFile.nLen - 1);

  // 4. 如果找到局面，则向前查找第一个着法；
  for (nPtr --; nPtr >= 0; nPtr --) {
    BookFile.Read(bk, nPtr);
    if (BOOK_POS_CMP(bk, posScan) < 0) {
      break;
    }
  }

  // 5. 向后依次读入属于该局面的每个着法；
  nMoves = 0;
  for (nPtr ++; nPtr < BookFile.nLen; nPtr ++) {
    BookFile.Read(bk, nPtr);
    if (BOOK_POS_CMP(bk, posScan) > 0) {
      break;
    }
    if (posScan.LegalMove(bk.wmv)) {
      // 如果局面是第二趟搜索到的，则着法必须做镜像
      lpbks[nMoves].nPtr = nPtr;
      lpbks[nMoves].wmv = (nScan == 0 ? bk.wmv : MOVE_MIRROR(bk.wmv));
      lpbks[nMoves].wvl = bk.wvl;
      nMoves ++;
      if (nMoves == MAX_GEN_MOVES) {
        break;
      }
    }
  }
  BookFile.Close();

  // 6. 对着法按分值排序
  for (i = 0; i < nMoves - 1; i ++) {
    for (j = nMoves - 1; j > i; j --) {
      if (lpbks[j - 1].wvl < lpbks[j].wvl) {
        SWAP(lpbks[j - 1], lpbks[j]);
      }
    }
  }
  return nMoves;
}
