/*
CHE->PGN Convertor - a Chinese Chess Score Convertion Program
Designed by Morning Yellow, Version: 3.14, Last Modified: Jun. 2008
Copyright (C) 2004-2008 www.elephantbase.net

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
#else
  #include <dlfcn.h>
#endif
#include "../base/base2.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "../cchess/ecco.h"
#include "../cchess/pgnfile.h"

static int ReadInt(FILE *fp) {
  int nResult, n;
  nResult = 0;
  n = fgetc(fp) - '0';
  while (n >= 0 && n <= 9) {
    nResult *= 10;
    nResult += n;
    n = fgetc(fp) - '0';
  }
  return nResult;
}

static const int CHE2PGN_ERROR_OPEN = -2;
static const int CHE2PGN_ERROR_CREATE = -1;
static const int CHE2PGN_OK = 0;

int Che2Pgn(const char *szCheFile, const char *szPgnFile, const EccoApiStruct &EccoApi) {
  int i, nMoveNum, mv, nStatus;
  int xSrc, ySrc, xDst, yDst;
  PgnFileStruct pgn;
  PositionStruct pos;
  FILE *fp;
  uint32_t dwEccoIndex, dwFileMove[20];

  fp = fopen(szCheFile, "rb");
  if (fp == NULL) {
    return CHE2PGN_ERROR_OPEN;
  }

  pgn.posStart.FromFen(cszStartFen);
  pos = pgn.posStart;
  ReadInt(fp);
  nMoveNum = ReadInt(fp);

  for (i = 0; i < nMoveNum; i ++) {
    ReadInt(fp);
    ReadInt(fp);
    ReadInt(fp);
    ySrc = 12 - ReadInt(fp);
    xSrc = ReadInt(fp) + 3;
    yDst = 12 - ReadInt(fp);
    xDst = ReadInt(fp) + 3;
    ReadInt(fp);
    ReadInt(fp);
    ReadInt(fp);

    mv = MOVE(COORD_XY(xSrc, ySrc), COORD_XY(xDst, yDst));
    mv &= 0xffff; // 防止TryMove时数组越界
    pgn.nMaxMove ++;
    if (pgn.nMaxMove <= 20) {
      dwFileMove[pgn.nMaxMove - 1] = Move2File(mv, pos);
    }
    // QQ象棋允许把将吃掉，但ElephantEye不允许，所以跳过非法着法
    if (TryMove(pos, nStatus, mv)) {
      pgn.wmvMoveTable[pgn.nMaxMove] = mv;
    } else {
      pgn.nMaxMove --;
    }
    if (pos.nMoveNum == MAX_MOVE_NUM) {
      pos.SetIrrev();
    }
  }

  if (pgn.nMaxMove < 20) {
    dwFileMove[pgn.nMaxMove] = 0;
  }
  if (EccoApi.Available()) {
    dwEccoIndex = EccoApi.EccoIndex((const char *) dwFileMove);
    strcpy(pgn.szEcco, (const char *) &dwEccoIndex);
    strcpy(pgn.szOpen, EccoApi.EccoOpening(dwEccoIndex));
    strcpy(pgn.szVar, EccoApi.EccoVariation(dwEccoIndex));
  }

  fclose(fp);
  return (pgn.Write(szPgnFile) ? CHE2PGN_OK : CHE2PGN_ERROR_CREATE);
}

#ifndef MXQFCONV_EXE

int main(int argc, char **argv) {
  EccoApiStruct EccoApi;
  char szLibEccoPath[1024];

  if (argc < 2) {
    printf("=== CHE->PGN Convertor ===\n");
    printf("Usage: CHE2PGN CHE-File [PGN-File]\n");
    return 0;
  }

  PreGenInit();
  ChineseInit();
  LocatePath(szLibEccoPath, cszLibEccoFile);
  EccoApi.Startup(szLibEccoPath);

  switch (Che2Pgn(argv[1], argc == 2 ? "CHE2PGN.PGN" : argv[2], EccoApi)) {
  case CHE2PGN_ERROR_OPEN:
    printf("%s: File Opening Error!\n", argv[1]);
    break;
  case CHE2PGN_ERROR_CREATE:
    printf("File Creation Error!\n");
    break;
  case CHE2PGN_OK:
#ifdef _WIN32
    if (argc == 2) {
      ShellExecute(NULL, NULL, "CHE2PGN.PGN", NULL, NULL, SW_SHOW);
    }
#endif
    break;
  }
  EccoApi.Shutdown();
  return 0;
}

#endif
