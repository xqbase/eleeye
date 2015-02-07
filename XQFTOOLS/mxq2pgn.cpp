/*
MXQ->PGN Convertor - a Chinese Chess Score Convertion Program
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
#include "../base/parse.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "../cchess/ecco.h"
#include "../cchess/pgnfile.h"

inline void ReadRecord(FILE *fp, char *sz) {
  uint8_t ucLen;
  fread(&ucLen, 1, 1, fp);
  fread(sz, 1, ucLen, fp);
  sz[ucLen] = '\0';
}

static const int MXQ2PGN_ERROR_OPEN = -2;
static const int MXQ2PGN_ERROR_CREATE = -1;
static const int MXQ2PGN_OK = 0;

int Mxq2Pgn(const char *szMxqFile, const char *szPgnFile, const EccoApiStruct &EccoApi) {
  int i, mv, nStatus;
  char *lpEvent;
  char szRecord[256], szComment[256];
  PgnFileStruct pgn;
  PositionStruct pos;

  FILE *fp;
  uint32_t dwEccoIndex, dwFileMove[20];

  fp = fopen(szMxqFile, "rb");
  if (fp == NULL) {
    return MXQ2PGN_ERROR_OPEN;
  }

  ReadRecord(fp, pgn.szSite);
  ReadRecord(fp, pgn.szDate);
  ReadRecord(fp, pgn.szEvent);
  lpEvent = pgn.szEvent;
  if (false) {
  } else if (StrScanSkip(lpEvent, "-胜-")) {
    pgn.nResult = 1;
  } else if (StrScanSkip(lpEvent, "-衊-")) {
    pgn.nResult = 1;
  } else if (StrScanSkip(lpEvent, "-和-")) {
    pgn.nResult = 2;
  } else if (StrScanSkip(lpEvent, "-㎝-")) {
    pgn.nResult = 2;
  } else if (StrScanSkip(lpEvent, "-负-")) {
    pgn.nResult = 3;
  } else if (StrScanSkip(lpEvent, "-璽-")) {
    pgn.nResult = 3;
  } else if (StrScanSkip(lpEvent, "-負-")) {
    pgn.nResult = 3;
  } else {
    pgn.nResult = 0;
  }
  if (pgn.nResult != 0) {
    strcpy(pgn.szRed, pgn.szEvent);
    *(pgn.szRed + (lpEvent - pgn.szEvent - 4)) = '\0';
    strcpy(pgn.szBlack, lpEvent);
  }
  ReadRecord(fp, pgn.szRedElo);
  ReadRecord(fp, pgn.szBlackElo);
  for (i = 0; i < 5; i ++) {
    ReadRecord(fp, szRecord);
  }
  ReadRecord(fp, szComment);
  ReadRecord(fp, szRecord);

  pgn.posStart.FromFen(cszStartFen);
  pos = pgn.posStart;

  ReadRecord(fp, szRecord);
  while (!StrEqv(szRecord, "Ends") && pgn.nMaxMove < MAX_MOVE_LEN - 1) {
    mv = MOVE(COORD_XY(szRecord[0] - '0' + 3, 'J' - szRecord[1] + 3), COORD_XY(szRecord[3] - '0' + 3, 'J' - szRecord[4] + 3));
    mv &= 0xffff; // 防止TryMove时数组越界
    pgn.nMaxMove ++;
    if (pgn.nMaxMove <= 20) {
      dwFileMove[pgn.nMaxMove - 1] = Move2File(mv, pos);
    }
    // 弈天可能允许把将吃掉，但ElephantEye不允许，所以跳过非法着法
    if (TryMove(pos, nStatus, mv)) {
      pgn.wmvMoveTable[pgn.nMaxMove] = mv;
    } else {
      pgn.nMaxMove --;
    }
    if (pos.nMoveNum == MAX_MOVE_NUM) {
      pos.SetIrrev();
    }
    ReadRecord(fp, szRecord);
  }
  pgn.szCommentTable[pgn.nMaxMove] = new char[256];
  strcpy(pgn.szCommentTable[pgn.nMaxMove], szComment);

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
  return (pgn.Write(szPgnFile) ? MXQ2PGN_OK : MXQ2PGN_ERROR_CREATE);
}

#ifndef MXQFCONV_EXE

int main(int argc, char **argv) {
  EccoApiStruct EccoApi;
  char szLibEccoPath[1024];

  if (argc < 2) {
    printf("=== MXQ->PGN Convertor ===\n");
    printf("Usage: MXQ2PGN MXQ-File [PGN-File]\n");
    return 0;
  }

  PreGenInit();
  ChineseInit();
  LocatePath(szLibEccoPath, cszLibEccoFile);
  EccoApi.Startup(szLibEccoPath);

  switch (Mxq2Pgn(argv[1], argc == 2 ? "MXQ2PGN.PGN" : argv[2], EccoApi)) {
  case MXQ2PGN_ERROR_OPEN:
    printf("%s: File Opening Error!\n", argv[1]);
    break;
  case MXQ2PGN_ERROR_CREATE:
    printf("File Creation Error!\n");
    break;
  case MXQ2PGN_OK:
#ifdef _WIN32
    if (argc == 2) {
      ShellExecute(NULL, NULL, "MXQ2PGN.PGN", NULL, NULL, SW_SHOW);
    }
#endif
    break;
  }
  EccoApi.Shutdown();
  return 0;
}

#endif
