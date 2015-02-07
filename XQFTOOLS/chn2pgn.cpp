/*
CHN->PGN Convertor - a Chinese Chess Score Convertion Program
Designed by Morning Yellow, Version: 3.14, Last Modified: Jun. 2008
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
#else
  #include <dlfcn.h>
#endif
#include "../base/base2.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "../cchess/ecco.h"
#include "../cchess/pgnfile.h"

static const int CHN2PGN_ERROR_OPEN = -2;
static const int CHN2PGN_ERROR_CREATE = -1;
static const int CHN2PGN_OK = 0;

struct ChnRecord {
  uint16_t wReserved1[2];
  uint16_t wxSrc, wySrc, wxDst, wyDst;
  uint16_t wReserved2[10];
};

int Chn2Pgn(const char *szChnFile, const char *szPgnFile, const EccoApiStruct &EccoApi) {
  int mv, nStatus;
  bool bRead, bFlip;
  PgnFileStruct pgn;
  PositionStruct pos;
  ChnRecord Chn;
  FILE *fp;
  uint32_t dwEccoIndex, dwFileMove[20];

  fp = fopen(szChnFile, "rb");
  if (fp == NULL) {
    return CHN2PGN_ERROR_OPEN;
  }

  pgn.posStart.FromFen(cszStartFen);
  pos = pgn.posStart;

  bRead = bFlip = false;
  fseek(fp, 188, SEEK_SET);
  while (fread(&Chn, sizeof(ChnRecord), 1, fp) > 0) {
    if (!bRead) {
      bRead = true;
      if (Chn.wySrc < 5) {
        bFlip = true;
      }
    }
    if (bFlip) {
      Chn.wxSrc = 8 - Chn.wxSrc;
      Chn.wySrc = 9 - Chn.wySrc;
      Chn.wxDst = 8 - Chn.wxDst;
      Chn.wyDst = 9 - Chn.wyDst;
    }
    mv = MOVE(COORD_XY(Chn.wxSrc + FILE_LEFT, Chn.wySrc + RANK_TOP),
        COORD_XY(Chn.wxDst + FILE_LEFT, Chn.wyDst + RANK_TOP));
    mv &= 0xffff; // 防止TryMove时数组越界
    pgn.nMaxMove ++;
    if (pgn.nMaxMove <= 20) {
      dwFileMove[pgn.nMaxMove - 1] = Move2File(mv, pos);
    }
    // 联众可能允许把将吃掉，但ElephantEye不允许，所以跳过非法着法
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
  return (pgn.Write(szPgnFile) ? CHN2PGN_OK : CHN2PGN_ERROR_CREATE);
}

#ifndef MXQFCONV_EXE

int main(int argc, char **argv) {
  EccoApiStruct EccoApi;
  char szLibEccoPath[1024];

  if (argc < 2) {
    printf("=== CHN->PGN Convertor ===\n");
    printf("Usage: CHN2PGN CHN-File [PGN-File]\n");
    return 0;
  }

  PreGenInit();
  ChineseInit();
  LocatePath(szLibEccoPath, cszLibEccoFile);
  EccoApi.Startup(szLibEccoPath);

  switch (Chn2Pgn(argv[1], argc == 2 ? "CHN2PGN.PGN" : argv[2], EccoApi)) {
  case CHN2PGN_ERROR_OPEN:
    printf("%s: File Opening Error!\n", argv[1]);
    break;
  case CHN2PGN_ERROR_CREATE:
    printf("File Creation Error!\n");
    break;
  case CHN2PGN_OK:
#ifdef _WIN32
    if (argc == 2) {
      ShellExecute(NULL, NULL, "CHN2PGN.PGN", NULL, NULL, SW_SHOW);
    }
#endif
    break;
  }
  EccoApi.Shutdown();
  return 0;
}

#endif
