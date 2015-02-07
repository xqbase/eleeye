/* 
ElephantEye Book Maker - a Chinese Chess Book Maker Program for ElephantEye
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

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "../base/base.h"
#include "../eleeye/position.h"
#include "../eleeye/book.h"
#include "../cchess/cchess.h"
#include "../cchess/pgnfile.h"

extern "C" __declspec(dllexport) BOOL WINAPI MakeBook(LPCSTR szPath, LPCSTR szFile,
    LONG nWin, LONG nDraw, LONG nLoss, LONG nRatio);
extern "C" __declspec(dllexport) LONG WINAPI GetBookMoves(const PositionStruct *lppos,
    LPCSTR szFile, BookStruct *lpbks);
extern "C" __declspec(dllexport) BOOL WINAPI PutBookMove(LPCSTR szFile, const BookStruct *lpcbk);

struct TempStruct {
  uint32_t dwZobristLock0, dwZobristLock1;
  int mv, vl;
  TempStruct() {
  }
  TempStruct(const TempStruct &tmp) {
    dwZobristLock0 = tmp.dwZobristLock0;
    dwZobristLock1 = tmp.dwZobristLock1;
    mv = tmp.mv;
    vl = tmp.vl;
  }
  TempStruct(const PositionStruct &pos, int mvArg, int vlArg) {
    dwZobristLock0 = pos.zobr.dwLock0;
    dwZobristLock1 = pos.zobr.dwLock1;
    mv = mvArg;
    vl = vlArg;
  }
  bool operator <(TempStruct tmp) const {
    return dwZobristLock1 < tmp.dwZobristLock1 ? true :
        dwZobristLock1 > tmp.dwZobristLock1 ? false :
        dwZobristLock0 < tmp.dwZobristLock0 ? true :
        dwZobristLock0 > tmp.dwZobristLock0 ? false : mv < tmp.mv;
  }
};

static struct {
  FILE *fpBookFile, *fpTempFile;
  int nTempLen;
  int nWin, nDraw, nLoss, nRatio;
  TempStruct *TempBuffer;
} MakeBook2;

inline void ReadTemp(int nPtr, TempStruct *lptmp, int nLen) {
  fseek(MakeBook2.fpTempFile, nPtr * sizeof(TempStruct), SEEK_SET);
  fread(lptmp, sizeof(TempStruct), nLen, MakeBook2.fpTempFile);
}

inline void WriteTemp(int nPtr, const TempStruct *lptmp, int nLen) {
  fseek(MakeBook2.fpTempFile, nPtr * sizeof(TempStruct), SEEK_SET);
  fwrite(lptmp, sizeof(TempStruct), nLen, MakeBook2.fpTempFile);
}

inline TempStruct GetTemp(int nPtr) {
  TempStruct tmp;
  ReadTemp(nPtr, &tmp, 1);
  return tmp;
}

inline void SetTemp(int nPtr, const TempStruct &tmp) {
  WriteTemp(nPtr, &tmp, 1);
}

inline void AddTemp(const TempStruct &tmp) {
  SetTemp(MakeBook2.nTempLen, tmp);
  MakeBook2.nTempLen ++;
}

inline int MoveValue(int sd, int nResult) {
  switch (nResult) {
  case 1:
    return sd == 0 ? MakeBook2.nWin : MakeBook2.nLoss;
  case 2:
    return MakeBook2.nDraw;
  case 3:
    return sd == 0 ? MakeBook2.nLoss : MakeBook2.nWin;
  default:
    return 0;
  }
}

static void ParseFile(const char *szFilePath) {
  int i, mv, mvMirror, nComp;
  PositionStruct pos, posMirror;
  PgnFileStruct pgn;

  if (pgn.Read(szFilePath)) {
    pos = posMirror = pgn.posStart;
    posMirror.Mirror();
    for (i = 0; i < pgn.nMaxMove; i ++) {
      mv = pgn.wmvMoveTable[i + 1];
      mvMirror = MOVE_MIRROR(mv);
      if (pos.zobr.dwLock1 < posMirror.zobr.dwLock1) {
        nComp = -1;
      } else if (pos.zobr.dwLock1 > posMirror.zobr.dwLock1) {
        nComp = 1;
      } else {
        if (pos.zobr.dwLock0 < posMirror.zobr.dwLock0) {
          nComp = -1;
        } else if (pos.zobr.dwLock0 > posMirror.zobr.dwLock0) {
          nComp = 1;
        } else {
          nComp = 0;
        }
      }
      if (nComp <= 0) {
        AddTemp(TempStruct(pos, mv, MoveValue(pos.sdPlayer, pgn.nResult)));
      }
      if (nComp >= 0) {
        AddTemp(TempStruct(posMirror, mvMirror, MoveValue(pos.sdPlayer, pgn.nResult)));
      }
      if (pos.ucpcSquares[DST(mv)] == 0) {
        pos.MakeMove(mv);
      } else {
        pos.MakeMove(mv);
        pos.SetIrrev();
      }
      if (posMirror.ucpcSquares[DST(mvMirror)] == 0) {
        posMirror.MakeMove(mvMirror);
      } else {
        posMirror.MakeMove(mvMirror);
        posMirror.SetIrrev();
      }  
    }  
  }
}

static void SearchFolder(const char *szFolderPath);

static void SearchFile(const char *szFilePath, const WIN32_FIND_DATA &wfd) {
  if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
    if (strlen(szFilePath) > 4) {
      if (strnicmp(szFilePath + strlen(szFilePath) - 4, ".PGN", 4) == 0) {
        ParseFile(szFilePath);
      }
    }
  } else {
    if (strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0) {
      SearchFolder(szFilePath);
    }
  }
}

static void SearchFolder(const char *szFolderPath) {
  char szFilePath[1024];
  WIN32_FIND_DATA wfd;
  HANDLE hFind;
  char *lpFilePath;

  strcpy(szFilePath, szFolderPath);
  lpFilePath = szFilePath + strlen(szFolderPath);
  if (*(lpFilePath - 1) == '\\') {
    lpFilePath --;
  }
  strcpy(lpFilePath, "\\*");
  lpFilePath ++;
  hFind = FindFirstFile(szFilePath, &wfd);
  if (hFind != INVALID_HANDLE_VALUE) {
    strcpy(lpFilePath, wfd.cFileName);
    SearchFile(szFilePath, wfd);
    while (FindNextFile(hFind, &wfd)) {
      strcpy(lpFilePath, wfd.cFileName);
      SearchFile(szFilePath, wfd);
    }
  }
  FindClose(hFind);
}

static const int TEMP_BUFFER_SIZE = 1048576;

static void SortMem(int low, int high) {
  TempStruct tmpMid, tmpSwap;
  int lowMid, highMid;

  lowMid = low;
  highMid = high;
  tmpMid = MakeBook2.TempBuffer[(low + high) / 2];
  while (lowMid <= highMid) {
    while (lowMid < high && MakeBook2.TempBuffer[lowMid] < tmpMid) {
      lowMid ++;
    }
    while (low < highMid && tmpMid < MakeBook2.TempBuffer[highMid]) {
      highMid --;
    }
    if (lowMid <= highMid) {
      SWAP(MakeBook2.TempBuffer[lowMid], MakeBook2.TempBuffer[highMid]);
      lowMid ++;
      highMid --;
    }
  }

  if (low < highMid) {
    SortMem(low, highMid);
  }
  if (lowMid < high) {
    SortMem(lowMid, high);
  }
}

static void SortTemp(int low, int high) {
  TempStruct tmpMid, tmpSwap;
  int lowMid, highMid, nLen;

  nLen = high - low + 1;
  if (nLen > TEMP_BUFFER_SIZE) {
    lowMid = low;
    highMid = high;
    tmpMid = GetTemp((low + high) / 2);
    while (lowMid <= highMid) {
      while (lowMid < high && GetTemp(lowMid) < tmpMid) {
        lowMid ++;
      }
      while (low < highMid && tmpMid < GetTemp(highMid)) {
        highMid --;
      }
      if (lowMid <= highMid) {
        tmpSwap = GetTemp(lowMid);
        SetTemp(lowMid, GetTemp(highMid));
        SetTemp(highMid, tmpSwap);
        lowMid ++;
        highMid --;
      }
    }

    if (low < highMid) {
      SortTemp(low, highMid);
    }
    if (lowMid < high) {
      SortTemp(lowMid, high);
    }

  } else {
    ReadTemp(low, MakeBook2.TempBuffer, nLen);
    if (nLen > 1) {
      SortMem(0, nLen - 1);
    }
    WriteTemp(low, MakeBook2.TempBuffer, nLen);
  }
}

static void MakeBook() {
  int i;
  TempStruct tmpLast, tmp;
  BookStruct bk;

  tmpLast.dwZobristLock0 = tmpLast.dwZobristLock1 = 0;
  tmpLast.mv = tmpLast.vl = 0;
  for (i = 0; i < MakeBook2.nTempLen; i ++) {
    tmp = GetTemp(i);
    if (tmpLast < tmp) {
      if (tmpLast.vl >= MakeBook2.nRatio) {
        bk.dwZobristLock = tmpLast.dwZobristLock1;
        bk.wmv = tmpLast.mv;
        bk.wvl = tmpLast.vl / MakeBook2.nRatio;
        fwrite(&bk, sizeof(BookStruct), 1, MakeBook2.fpBookFile);
      }
      tmpLast = tmp;
    } else {
      tmpLast.vl += tmp.vl;
    }
  }
  if (tmpLast.vl >= MakeBook2.nRatio) {
    bk.dwZobristLock = tmpLast.dwZobristLock1;
    bk.wmv = tmpLast.mv;
    bk.wvl = tmpLast.vl / MakeBook2.nRatio;
    fwrite(&bk, sizeof(BookStruct), 1, MakeBook2.fpBookFile);
  }
}

static bool bInit = false;

inline void Init(void) {
  if (!bInit) {
    bInit = true;
    PreGenInit();
    ChineseInit();
  }
}

BOOL WINAPI MakeBook(LPCSTR szPath, LPCSTR szFile,
    LONG nWin, LONG nDraw, LONG nLoss, LONG nRatio) {
  BOOL bSuccess;
  Init();
  bSuccess = FALSE;
  MakeBook2.nWin = nWin;
  MakeBook2.nDraw = nDraw;
  MakeBook2.nLoss = nLoss;
  MakeBook2.nRatio = nRatio;
  MakeBook2.fpTempFile = tmpfile();
  if (MakeBook2.fpTempFile != NULL) {
    MakeBook2.nTempLen = 0;
    MakeBook2.fpBookFile = fopen(szFile, "wb");
    if (MakeBook2.fpBookFile != NULL) {
      SearchFolder(szPath);
      if (MakeBook2.nTempLen > 1) {
        MakeBook2.TempBuffer = new TempStruct[TEMP_BUFFER_SIZE];
        SortTemp(0, MakeBook2.nTempLen - 1);
        delete[] MakeBook2.TempBuffer;
      }
      MakeBook();
      fclose(MakeBook2.fpBookFile);
      bSuccess = TRUE;
    }
    fclose(MakeBook2.fpTempFile);
  }
  return bSuccess;
}

LONG WINAPI GetBookMoves(const PositionStruct *lppos,
    LPCSTR szFile, BookStruct *lpbks) {
  Init();
  return GetBookMoves(*lppos, szFile, lpbks);
}

BOOL WINAPI PutBookMove(LPCSTR szFile, const BookStruct *lpcbk) {
  BookFileStruct BookFile;
  BookStruct bk;
  int nPtr;

  Init();
  nPtr = lpcbk->nPtr;
  if (!BookFile.Open(szFile, true)) {
    return FALSE;
  }
  if (nPtr >= BookFile.nLen) {
    BookFile.Close();
    return FALSE;
  }
  BookFile.Read(bk, nPtr);
  bk.wvl = lpcbk->wvl;
  BookFile.Write(bk, nPtr);
  BookFile.Close();
  return TRUE;
}