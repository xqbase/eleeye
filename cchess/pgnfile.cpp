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

#include <stdio.h>
#include <string.h>
#include "../base/base.h"
#include "../base/parse.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "pgnfile.h"

static const char *const cszResult[4] = {
  "*", "1-0", "1/2-1/2", "0-1"
};

void PgnFileStruct::Init(void) {
  int i;
  szEvent[0] = szRound[0] = szDate[0] = szSite[0] = '\0';
  szRedTeam[0] = szRed[0] = szRedElo[0] = '\0';
  szBlackTeam[0] = szBlack[0] = szBlackElo[0] = '\0';
  szEcco[0] = szOpen[0] = szVar[0] = '\0';
  nMaxMove = nResult = 0;
  posStart.ClearBoard();
  posStart.SetIrrev();
  for (i = 0; i < MAX_MOVE_LEN; i ++) {
    szCommentTable[i] = NULL;
  }  
}

PgnFileStruct::~PgnFileStruct(void) {
  int i;
  for (i = 0; i < MAX_MOVE_LEN; i ++) {
    if (szCommentTable[i] != NULL) {
      delete[] szCommentTable[i];
    }
  }
}

static bool GetLabel(char *szDestStr, const char *szLineStr, const char *szLabelName) {
  int nValueLen;
  const char *lpLabelEnd;
  char szTempLabel[MAX_STR_LEN];
  strcpy(szTempLabel, "[");
  strcat(szTempLabel, szLabelName);
  strcat(szTempLabel, " \"");
  if (StrEqvSkip(szLineStr, szTempLabel)) {
    lpLabelEnd = strchr(szLineStr, '\"');
    if (lpLabelEnd == NULL) {
      nValueLen = strlen(szLineStr) - 1;
    } else {
      nValueLen = lpLabelEnd - szLineStr;
    }
    if (nValueLen >= MAX_STR_LEN) {
      nValueLen = MAX_STR_LEN - 1;
    }
    strncpy(szDestStr, szLineStr, nValueLen);
    szDestStr[nValueLen] = '\0';
    return true;
  } else {
    return false;
  }
}

static void AppendStr(char *&szDst, const char *szSrc) {
  int nDstLen;
  if (szDst == NULL) {
    szDst = new char[MAX_REM_LEN];
    szDst[0] = '\0';
  }
  nDstLen = strlen(szDst);
  strncpy(szDst + nDstLen, szSrc, MAX_REM_LEN - nDstLen - 1);
  szDst[MAX_REM_LEN - 1] = '\0';
}

inline int ICCS_MOVE(const char *szIccsStr) {
  int sqSrc, sqDst;
  sqSrc = COORD_XY(szIccsStr[0] - 'A' + FILE_LEFT, '9' + RANK_TOP - szIccsStr[1]);
  sqDst = COORD_XY(szIccsStr[3] - 'A' + FILE_LEFT, '9' + RANK_TOP - szIccsStr[4]);
  return MOVE(sqSrc, sqDst);
}

static const char *const cszAdvertStr = "\r\n"
    "============================\r\n"
    " 欢迎访问《象棋百科全书网》 \r\n"
    " 推荐用《象棋巫师》观赏棋谱 \r\n"
    "http://www.elephantbase.net/\r\n";

bool PgnFileStruct::Read(const char *szFileName, bool bNoAdvert) {
  FILE *fp;
  int nRemLevel, nRemLen;
  bool bReturned, bDetail, bEndFor;
  int nNotation, nCounter, nStatus, mv;
  const char *lpLineChar;
  char szLineStr[MAX_STR_LEN], szRem[MAX_REM_LEN];
  PositionStruct pos;

  Reset();
  fp = fopen(szFileName, "rb");
  if (fp == NULL) {
    return false;
  }
  posStart.FromFen(cszStartFen);
  bReturned = true;
  bDetail = false;
  nRemLevel = 0;
  nRemLen = 0;
  nNotation = 0;
  nCounter = 1;
  pos = posStart;
  lpLineChar = NULL;
  while (!(feof(fp) && bReturned)) {
    if (bReturned) {
      fgets(szLineStr, 256, fp);
      lpLineChar = szLineStr;
      bReturned = false;
    }
    if (bDetail) {
      if (nRemLevel > 0) {
        bEndFor = true;
        while (*lpLineChar != '\0' && *lpLineChar != '\r' && *lpLineChar != '\n') {
          nRemLevel += (*lpLineChar == '(' || *lpLineChar == '{' ? 1 : *lpLineChar == ')' || *lpLineChar == '}' ? -1 : 0);
          if (nRemLevel == 0) {
            szRem[nRemLen] = '\0';
            AppendStr(szCommentTable[nMaxMove], szRem);
            nRemLen = 0;
            bEndFor = false;
            lpLineChar ++;
            break;
          } else {
            if (nRemLen < MAX_STR_LEN) {
              szRem[nRemLen] = *lpLineChar;
              nRemLen ++;
            }
          }
          lpLineChar ++;
        }
        if (bEndFor) {
          szRem[nRemLen] = '\0';
          AppendStr(szCommentTable[nMaxMove], szRem);
          nRemLen = 0;
          if (*lpLineChar == '\r' || *lpLineChar == '\n') {
            AppendStr(szCommentTable[nMaxMove], "\r\n");
          }
          bReturned = true;
        }
      } else {
        bEndFor = true;
        while (*lpLineChar != '\0' && *lpLineChar != '\r' && *lpLineChar != '\n') {
          switch (*lpLineChar) {
          case '(':
          case '{':
            nRemLevel ++;
            bEndFor = false;
            break;
          case '0':
            if (strncmp(lpLineChar, "0-1", 3) == 0) {
              if (!bNoAdvert) {
                AppendStr(szCommentTable[nMaxMove], cszAdvertStr);
              }
              fclose(fp);
              return true;
            }
            break;
          case '1':
            if (strncmp(lpLineChar, "1-0", 3) == 0 || strncmp(lpLineChar, "1/2-1/2", 7) == 0) {
              if (!bNoAdvert) {
                AppendStr(szCommentTable[nMaxMove], cszAdvertStr);
              }
              fclose(fp);
              return true;
            }
            break;
          case '*':
            if (!bNoAdvert) {
              AppendStr(szCommentTable[nMaxMove], cszAdvertStr);
            }
            fclose(fp);
            return true;
            break;
          default:
            if (nNotation > 0) {
              if ((*lpLineChar >= 'A' && *lpLineChar <= 'Z') || (*lpLineChar >= 'a' && *lpLineChar <= 'z') || *lpLineChar == '+' || *lpLineChar == '-' || *lpLineChar == '=') {
                if (nNotation == 1) {
                  mv = File2Move(*(long *) lpLineChar, pos);
                } else {
                  mv = ICCS_MOVE(lpLineChar);
                }
                if (TryMove(pos, nStatus, mv)) {
                  if (pos.nMoveNum == MAX_MOVE_NUM) {
                    pos.SetIrrev();
                  }
                  if (nCounter < MAX_COUNTER) {
                    if (pos.sdPlayer == 0) {
                      nCounter ++;
                    }
                    nMaxMove ++;
                    wmvMoveTable[nMaxMove] = mv;
                    if (nNotation == 1) {
                      lpLineChar += 3;
                    } else {
                      lpLineChar += 5;
                    }
                  }
                }
                bEndFor = false;
                break;
              }
            } else {
              if (*lpLineChar < 0) {
                mv = File2Move(Chin2File(*(uint64_t *) lpLineChar), pos);
                if (TryMove(pos, nStatus, mv)) {
                  if (pos.nMoveNum == MAX_MOVE_NUM) {
                    pos.SetIrrev();
                  }
                  if (nCounter < MAX_COUNTER) {
                    if (pos.sdPlayer == 0) {
                      nCounter ++;
                    }
                    nMaxMove ++;
                    wmvMoveTable[nMaxMove] = mv;
                    lpLineChar += 7;
                  }
                }
              }
            }
            break;
          }
          lpLineChar ++;
          if (!bEndFor) {
            break;
          }
        }
        if (bEndFor) {
          bReturned = true;
        }
      }
    } else {
      if (szLineStr[0] == '\0') {
        bReturned = true;
      } else if (szLineStr[0] == '[') {
        if (false) {
        } else if (GetLabel(szEvent, szLineStr, "EVENT")) {
        } else if (GetLabel(szRound, szLineStr, "ROUND")) {
        } else if (GetLabel(szDate, szLineStr, "DATE")) {
        } else if (GetLabel(szSite, szLineStr, "SITE")) {
        } else if (GetLabel(szRedTeam, szLineStr, "REDTEAM")) {
        } else if (GetLabel(szRed, szLineStr, "RED")) {
        } else if (GetLabel(szRedElo, szLineStr, "REDELO")) {
        } else if (GetLabel(szBlackTeam, szLineStr, "BLACKTEAM")) {
        } else if (GetLabel(szBlack, szLineStr, "BLACK")) {
        } else if (GetLabel(szBlackElo, szLineStr, "BLACKELO")) {
        } else if (GetLabel(szRem, szLineStr, "RESULT")) {
          if (false) {
          } else if (strcmp(szRem, "*") == 0) {
            nResult = 0;
          } else if (strcmp(szRem, "1-0") == 0) {
            nResult = 1;
          } else if (strcmp(szRem, "1/2-1/2") == 0) {
            nResult = 2;
          } else if (strcmp(szRem, "0-1") == 0) {
            nResult = 3;
          } else {
            nResult = 0;
          }
        } else if (GetLabel(szEcco, szLineStr, "ECCO")) {
        } else if (GetLabel(szOpen, szLineStr, "OPENING")) {
        } else if (GetLabel(szVar, szLineStr, "VARIATION")) {
        } else if (GetLabel(szRem, szLineStr, "FORMAT")) {
          if (StrEqv(szRem, "WXF")) {
            nNotation = 1;
          } else if (StrEqv(szRem, "ICCS")) {
            nNotation = 2;
          } else {
            nNotation = 0;
          }
        } else if (GetLabel(szRem, szLineStr, "FEN")) {
          posStart.FromFen(szRem);
          lpLineChar = strchr(szRem, '-');
          pos = posStart;
          nCounter = 1;
        }
        bReturned = true;
      } else {
        bDetail = true;
      }
    }
  }
  if (!bNoAdvert) {
    AppendStr(szCommentTable[nMaxMove], cszAdvertStr);
  }
  fclose(fp);
  return true;
}

inline void PrintLabel(FILE *fp, const char *szTag, const char *szValue) {
  if (szValue[0] != '\0') {
    fprintf(fp, "[%s \"%s\"]\r\n", szTag, szValue);
  }
}

bool PgnFileStruct::Write(const char *szFileName, bool bNoAdvert) const {
  int i, nCounter, nStatus;
  bool bReturned;
  uint64_t dqChinMove;
  char szFen[128];
  FILE *fp;
  PositionStruct pos;

  fp = fopen(szFileName, "wb");
  if (fp == NULL) {
    return false;
  }
  PrintLabel(fp, "Game", "Chinese Chess");
  PrintLabel(fp, "Event", szEvent);
  PrintLabel(fp, "Round", szRound);
  PrintLabel(fp, "Date", szDate);
  PrintLabel(fp, "Site", szSite);
  PrintLabel(fp, "RedTeam", szRedTeam);
  PrintLabel(fp, "Red", szRed);
  PrintLabel(fp, "RedElo", szRedElo);
  PrintLabel(fp, "BlackTeam", szBlackTeam);
  PrintLabel(fp, "Black", szBlack);
  PrintLabel(fp, "BlackElo", szBlackElo);
  PrintLabel(fp, "Result", cszResult[nResult]);
  PrintLabel(fp, "ECCO", szEcco);
  PrintLabel(fp, "Opening", szOpen);
  PrintLabel(fp, "Variation", szVar);
  posStart.ToFen(szFen);
  if (strcmp(szFen, cszStartFen) != 0) {
    fprintf(fp, "[FEN \"%s - - 0 1\"]\r\n", szFen);
  }
  if (szCommentTable[0] != NULL) {
    fprintf(fp, "{%s}\r\n", szCommentTable[0]);
  }
  pos = posStart;
  bReturned = true;
  nCounter = 1;
  for (i = 1; i <= nMaxMove; i ++) {
    if (bReturned) {
      fprintf(fp, "%3d. ", nCounter);
      if (pos.sdPlayer == 1) {
        fprintf(fp, "        ");
      }
    }
    dqChinMove = File2Chin(Move2File(wmvMoveTable[i], pos), pos.sdPlayer);
    if (pos.sdPlayer == 0) {
      fprintf(fp, "%.8s", (const char *) &dqChinMove);
      bReturned = false;
    } else {
      fprintf(fp, " %.8s\r\n", (const char *) &dqChinMove);
      bReturned = true;
    }
    if (szCommentTable[i] != NULL) {
      if (!bReturned) {
        fprintf(fp, "\r\n");
      }
      fprintf(fp, "{%s}\r\n", szCommentTable[i]);
      bReturned = true;
    }
    TryMove(pos, nStatus, wmvMoveTable[i]);
    if (pos.nMoveNum == MAX_MOVE_NUM) {
      pos.SetIrrev();
    }
    if (pos.sdPlayer == 0) {
      nCounter ++;
      if (nCounter == MAX_COUNTER) {
        break;
      }
    }
  }
  fprintf(fp, " %s%s", cszResult[nResult], bNoAdvert ? "" : cszAdvertStr);
  fclose(fp);
  return true;
}
