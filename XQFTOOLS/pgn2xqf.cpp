/*
PGN->XQF Convertor - a Chinese Chess Score Convertion Program
Designed by Morning Yellow, Version: 2.02, Last Modified: Apr. 2007
Copyright (C) 2004-2007 www.elephantbase.net

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
#ifdef _WIN32
  #include <windows.h>
#endif
#include "../base/base.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "../cchess/pgnfile.h"
#include "xqffile.h"

static const char ccSquare2Xqf[256] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1,  9, 19, 29, 39, 49, 59, 69, 79, 89, -1, -1, -1, -1,
  -1, -1, -1,  8, 18, 28, 38, 48, 58, 68, 78, 88, -1, -1, -1, -1,
  -1, -1, -1,  7, 17, 27, 37, 47, 57, 67, 77, 87, -1, -1, -1, -1,
  -1, -1, -1,  6, 16, 26, 36, 46, 56, 66, 76, 86, -1, -1, -1, -1,
  -1, -1, -1,  5, 15, 25, 35, 45, 55, 65, 75, 85, -1, -1, -1, -1,
  -1, -1, -1,  4, 14, 24, 34, 44, 54, 64, 74, 84, -1, -1, -1, -1,
  -1, -1, -1,  3, 13, 23, 33, 43, 53, 63, 73, 83, -1, -1, -1, -1,
  -1, -1, -1,  2, 12, 22, 32, 42, 52, 62, 72, 82, -1, -1, -1, -1,
  -1, -1, -1,  1, 11, 21, 31, 41, 51, 61, 71, 81, -1, -1, -1, -1,
  -1, -1, -1,  0, 10, 20, 30, 40, 50, 60, 70, 80, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static const int cpcXqf2Piece[32] = {
  23, 21, 19, 17, 16, 18, 20, 22, 24, 25, 26, 27, 28, 29, 30, 31,
  39, 37, 35, 33, 32, 34, 36, 38, 40, 41, 42, 43, 44, 45, 46, 47
};

static const int cnResultTrans[4] = {
  0, 1, 3, 2
};

inline void SetXqfString(char *sz, const char *szValue, int nMaxLen) {
  int nLen;
  nLen = MIN((int) strlen(szValue), nMaxLen - 1);
  sz[0] = nLen;
  strncpy(sz + 1, szValue, nLen);
}

static const int PGN2XQF_ERROR_OPEN = -2;
static const int PGN2XQF_ERROR_CREATE = -1;
static const int PGN2XQF_OK = 0;

int Pgn2Xqf(const char *szPgnFile, const char *szXqfFile) {
  int i, nCommentLen;
  char szRed[MAX_STR_LEN * 2], szBlack[MAX_STR_LEN * 2];
  FILE *fp;
  PgnFileStruct pgn;
  XqfHeaderStruct xqfhd;
  XqfMoveStruct xqfmv;

  if (!pgn.Read(szPgnFile)) {
    return PGN2XQF_ERROR_OPEN;
  }
  fp = fopen(szXqfFile, "wb");
  if (fp == NULL) {
    return PGN2XQF_ERROR_CREATE;
  }
  memset(xqfhd.szTag, 0, sizeof(XqfHeaderStruct));
  xqfhd.szTag[0] = 'X';
  xqfhd.szTag[1] = 'Q';
  xqfhd.szTag[2] = 10;
  for (i = 0; i < 32; i ++) {
    xqfhd.szPiecePos[i] = ccSquare2Xqf[pgn.posStart.ucsqPieces[cpcXqf2Piece[i]]];
  }
  xqfhd.szResult[3] = cnResultTrans[pgn.nResult];
  xqfhd.szSetUp[0] = 2;
  SetXqfString(xqfhd.szEvent, pgn.szEvent, sizeof(xqfhd.szEvent));
  SetXqfString(xqfhd.szDate, pgn.szDate, sizeof(xqfhd.szDate));
  SetXqfString(xqfhd.szSite, pgn.szSite, sizeof(xqfhd.szSite));
  if (pgn.szRedTeam[0] == '\0') {
    SetXqfString(xqfhd.szRed, pgn.szRed, sizeof(xqfhd.szRed));
  } else {
    sprintf(szRed, "%s %s", pgn.szRedTeam, pgn.szRed);
    SetXqfString(xqfhd.szRed, szRed, sizeof(xqfhd.szRed));
  }
  if (pgn.szBlackTeam[0] == '\0') {
    SetXqfString(xqfhd.szBlack, pgn.szBlack, sizeof(xqfhd.szBlack));
  } else {
    sprintf(szBlack, "%s %s", pgn.szBlackTeam, pgn.szBlack);
    SetXqfString(xqfhd.szBlack, szBlack, sizeof(xqfhd.szBlack));
  }
  fwrite(&xqfhd, sizeof(xqfhd), 1, fp);
  memset(&xqfhd, 0, sizeof(xqfhd));
  fwrite(&xqfhd, sizeof(xqfhd), 1, fp);
  for (i = 0; i <= pgn.nMaxMove; i ++) {
    xqfmv.ucTag = (i == pgn.nMaxMove ? 0 : 240);
    if (i == 0) {
      xqfmv.ucSrc = 24;
      xqfmv.ucDst = 32;
      xqfmv.ucReserved = 255;
    } else {
      xqfmv.ucSrc = 24 + ccSquare2Xqf[SRC(pgn.wmvMoveTable[i])];
      xqfmv.ucDst = 32 + ccSquare2Xqf[DST(pgn.wmvMoveTable[i])];
      xqfmv.ucReserved = 0;
    }
    fwrite(&xqfmv, sizeof(XqfMoveStruct), 1, fp);
    nCommentLen = (pgn.szCommentTable[i] == NULL ? 0 : strlen(pgn.szCommentTable[i]));
    fwrite(&nCommentLen, sizeof(int), 1, fp);
    if (pgn.szCommentTable[i] != NULL) {
      fwrite(pgn.szCommentTable[i], nCommentLen, 1, fp);
    }
  }
  fclose(fp);
  return 0;
}

#ifndef MXQFCONV_EXE

int main(char argc, char **argv) {
  if (argc <= 1) {
    printf("=== PGN->XQF Convertor ===\n");
    printf("Usage: PGN2XQF PGN-File [XQF-File]\n");
    return 0;
  }
  PreGenInit();
  ChineseInit();

  switch (Pgn2Xqf(argv[1], argc == 2 ? "PGN2XQF.XQF" : argv[2])) {
  case PGN2XQF_ERROR_OPEN:
    printf("%s: File Not Found or Not a Chinese-Chess-PGN File!\n", argv[1]);
    break;
  case PGN2XQF_ERROR_CREATE:
    printf("File Creation Error!\n");
    break;
  case PGN2XQF_OK:
#ifdef _WIN32
    if (argc == 2) {
      ShellExecute(NULL, NULL, "PGN2XQF.XQF", NULL, NULL, SW_SHOW);
    }
#endif
    break;
  }
  return 0;
}

#endif
