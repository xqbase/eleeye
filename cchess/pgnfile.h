/* 
PGN->XQF - a Chinese Chess Score Convertion Program
Designed by Morning Yellow, Version: 2.1, Last Modified: Jun. 2007
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

#include "../cchess/cchess.h"

#ifndef PGNFILE_H
#define PGNFILE_H

const int MAX_STR_LEN = 256;
const int MAX_MOVE_LEN = 1999;
const int MAX_COUNTER = 1000;
const int MAX_REM_LEN = 4096;

const bool NO_ADVERT = true;

struct PgnFileStruct {
  char szEvent[MAX_STR_LEN], szRound[MAX_STR_LEN], szDate[MAX_STR_LEN], szSite[MAX_STR_LEN];
  char szRedTeam[MAX_STR_LEN], szRed[MAX_STR_LEN], szRedElo[MAX_STR_LEN];
  char szBlackTeam[MAX_STR_LEN], szBlack[MAX_STR_LEN], szBlackElo[MAX_STR_LEN];
  char szEcco[MAX_STR_LEN], szOpen[MAX_STR_LEN], szVar[MAX_STR_LEN];
  int nMaxMove, nResult;
  PositionStruct posStart;
  unsigned short wmvMoveTable[MAX_MOVE_LEN];
  char *szCommentTable[MAX_MOVE_LEN];

  void Init(void);
  PgnFileStruct(void) {
    Init();
  };
  ~PgnFileStruct(void);
  void Reset(void) {
    this->~PgnFileStruct();
    Init();
  }
  bool Read(const char *szFileName, bool bNoAdvert = false);
  bool Write(const char *szFileName, bool bNoAdvert = false) const;
}; // pgn

#endif
