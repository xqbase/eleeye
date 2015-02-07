/* 
Unit Test - for ElephantEye
Designed by Morning Yellow, Version: 3.12, Last Modified: Dec. 2007
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
#include "../eleeye/position.h"

const int MAX_CHAR = 1024;

int main(char argc, char **argv) {
  char szLineStr[MAX_CHAR];
  MoveStruct mvs[MAX_GEN_MOVES];
  PositionStruct pos;
  FILE *fp;
  int nPosNum, sqSrc, sqDst, i;
  int nThisLegal, nThisGened, nThisMoved, nThisCheck;
  int nTotalLegal, nTotalGened, nTotalMoved, nTotalCheck;

  if (argc <= 1) {
    printf("=== ElephantEye Unit Test Program ===\n");
    printf("Usage: UNITTEST EPD-File\n");
    return 0;
  }
  fp = fopen(argv[1], "rt");
  if (fp == NULL) {
    printf("%s: File Opening Error!\n", argv[1]);
  }
  PreGenInit();
  nPosNum = nTotalLegal = nTotalGened = nTotalMoved = nTotalCheck = 0;
  printf("    No Legal Gened Moved Check Position (FEN)\n");
  printf("===================================================\n");
  fflush(stdout);
  while (fgets(szLineStr, MAX_CHAR, fp) != NULL) {
    nPosNum ++;
    nThisLegal = nThisMoved = nThisCheck = 0;
    pos.FromFen(szLineStr);
    for (sqSrc = 0; sqSrc < 256; sqSrc ++) {
      if (IN_BOARD(sqSrc)) {
        for (sqDst = 0; sqDst < 256; sqDst ++) {
          if (IN_BOARD(sqDst) && sqDst != sqSrc) {
            nThisLegal += (pos.LegalMove(MOVE(sqSrc, sqDst)) ? 1 : 0);
          }
        }
      }
    }
    nThisGened = pos.GenAllMoves(mvs);
    for (i = 0; i < nThisGened; i ++) {
      if (pos.MakeMove(mvs[i].wmv)) {
        nThisMoved ++;
        nThisCheck += (pos.LastMove().ChkChs > 0 ? 1 : 0);
        pos.UndoMakeMove();
      }
    }
    pos.ToFen(szLineStr);
    printf("%6d%6d%6d%6d%6d %s\n", nPosNum, nThisLegal, nThisGened, nThisMoved, nThisCheck, szLineStr);
    fflush(stdout);
    nTotalLegal += nThisLegal;
    nTotalGened += nThisGened;
    nTotalMoved += nThisMoved;
    nTotalCheck += nThisCheck;
  }
  printf("====================================================\n");
  printf(" Total%6d%6d%6d%6d\n", nTotalLegal, nTotalGened, nTotalMoved, nTotalCheck);
  fflush(stdout);
  fclose(fp);
  return 0;
}
