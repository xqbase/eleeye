/* 
Pikafish Proxy - a Chinese Chess Engine Wrapper for XQWizard to run Pikafish
Designed by Morning Yellow, Version: 2023-03-05, Last Modified: Jun. 2023
Copyright (C) 2023 www.xqbase.com

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
#include "version.h"
#include "../base/pipe.h"
#include "../base/base.h"
#include "../base/base2.h"
#include "../base/rc4prng.h"
#include "../eleeye/pregen.h"
#include "../eleeye/position.h"
#include "../eleeye/book.h"

int main(void) {
  bool bUseBook;
  int i, nLen, mv, vl;
  uint32_t dwMoveStr;
  char *lp, *lpTime, *lp2;
  char sz[LINE_INPUT_MAX_CHAR], sz2[LINE_INPUT_MAX_CHAR];
  char szTime[LINE_INPUT_MAX_CHAR], szInc[LINE_INPUT_MAX_CHAR], szBookFile[LINE_INPUT_MAX_CHAR];
  BookStruct mvsBook[MAX_GEN_MOVES];
  RC4Struct rc4;
  PipeStruct pipeConsole, pipeEngine;
  PositionStruct pos;

  bUseBook = true;
  LocatePath(sz, "pikafish-modern.exe");
  LocatePath(szBookFile, "BOOK.DAT");
  pipeConsole.Open();
  pipeEngine.Open(sz);
  PreGenInit();
  rc4.InitRand();
  pos.FromFen(cszStartFen);

  while (pipeEngine.nEof == 0) {

    // Read Pikafish's Response
    while (pipeEngine.LineInput(sz)) {
      // printf("info string ENG->GUI: [%s]\n", sz);
      if (false) {

        // "Pikafish ...": Ignore
      } else if (strncmp(sz, "Pikafish ", 9) == 0) {

        // "id ...", "option ...", "uciok": Ignore
      } else if (strncmp(sz, "id ", 3) == 0) {
      } else if (strncmp(sz, "option ", 7) == 0) {
      } else if (strcmp(sz, "uciok") == 0) {

        // "bestmove (none)" -> "nobestmove"
      } else if (strcmp(sz, "bestmove (none)") == 0) {
        printf("nobestmove\n");

        // "info depth ... score cp ..." -> "info depth ... score ..."
      } else if (strncmp(sz, "info depth ", 11) == 0) {
        lp = strstr(sz, " score cp ");
        if (lp != NULL) {
          strcpy(sz2, lp + 10);
          strcpy(lp + 7, sz2);
        }
        printf("%s\n", sz);

        // else: unchanged
      } else {
        printf("%s\n", sz);
      }
      fflush(stdout);
    }

    // Read XQWizard's Request
    while (pipeConsole.LineInput(sz)) {
      if (false) {

        // "ucci" -> "uci", show version and options
      } else if (strcmp(sz, "ucci") == 0) {

        printf("id name Pikafish\n");
        printf("id version " PIKAFISH_VERSION "\n");
        printf("option usemillisec type check default true\n");
        printf("option usebook type check default true\n");
        printf("option bookfiles type string default %s\n", szBookFile);
        printf("option hashsize type spin default 16 min 1 max 33554432\n");
        printf("option threads type spin default 1 min 1 max 1024\n");
        printf("ucciok\n");
        strcpy(sz, "uci");

        // "setoption ...": only accepts "usebook", "bookfiles", "hashsize" and "threads"
      } else if (strncmp(sz, "setoption ", 10) == 0) {

        if (false) {

          // "setoption usebook true/false"
        } else if (strncmp(sz, "setoption usebook ", 18) == 0) {
          lp = sz + 18;
          bUseBook = (strcmp(lp, "true") == 0 || strcmp(lp, "on") == 0);
          continue;

          // "setoption bookfiles ..."
        } else if (strncmp(sz, "setoption bookfiles ", 20) == 0) {
          lp = sz + 20;
          if (AbsolutePath(lp)) {
            strcpy(szBookFile, lp);
          } else {
            LocatePath(szBookFile, lp);
          }
          continue;

          // "setoption hashsize ..." -> "setoption name Hash value ..."
        } else if (strncmp(sz, "setoption hashsize ", 19) == 0) {
          strcpy(sz2, "setoption name Hash value ");
          strcat(sz2, sz + 19);
          strcpy(sz, sz2);

          // "setoption threads ..." -> "setoption name Threads value ..."
        } else if (strncmp(sz, "setoption threads ", 18) == 0) {
          sscanf(sz + 18, "%d", &i);
          sprintf(sz, "setoption name Threads value %d", i < 1 ? 1 : i);
        } else {

          // else: unchanged
          continue;
        }

        // "position ...": parse position for book search
      } else if (strncmp(sz, "position ", 9) == 0) {

        lp = strstr(sz, " fen ");
        if (lp == NULL) {
          pos.FromFen(cszStartFen);
        } else {
          pos.FromFen(lp + 5);
        }
        lp = strstr(sz, " moves ");
        if (lp != NULL) {
          lp += 7;
          nLen = (strlen(lp) + 1) / 5;
          for (i = 0; i < nLen; i ++) {
            mv = COORD_MOVE(*(uint32_t *) lp);
            lp += 5;
            if (mv == 0) {
              break;
            }
            if (pos.ucpcSquares[SRC(mv)] == 0) {
              break;
            }
            pos.MakeMove(mv);
            if (pos.LastMove().CptDrw > 0) {
              pos.SetIrrev();
            }
          }
        }

        // "go ...": search book first, then call engine
      } else if (strncmp(sz, "go ", 3) == 0) {

        // search book
        if (bUseBook) {
          // a. get all moves for this position
          nLen = GetBookMoves(pos, szBookFile, mvsBook);

          if (nLen > 0) {
            vl = 0;
            for (i = 0; i < nLen; i ++) {
              vl += mvsBook[i].wvl;
              dwMoveStr = MOVE_COORD(mvsBook[i].wmv);
              printf("info depth 0 score %d pv %.4s\n", mvsBook[i].wvl, (const char *) &dwMoveStr);
              fflush(stdout);
            }

            // b. pick a random move by move weight
            vl = rc4.NextLong() % (uint32_t) vl;
            for (i = 0; i < nLen; i ++) {
              vl -= mvsBook[i].wvl;
              if (vl < 0) {
                break;
              }
            }

            // c. skip the move that causes repetition
            pos.MakeMove(mvsBook[i].wmv);
            if (pos.RepStatus(3) == 0) {
              dwMoveStr = MOVE_COORD(mvsBook[i].wmv);
              printf("bestmove %.4s", (const char *) &dwMoveStr);
              // d. get ponder move (next move with max weight)
              nLen = GetBookMoves(pos, szBookFile, mvsBook);
              pos.UndoMakeMove();
              if (nLen > 0) {
                dwMoveStr = MOVE_COORD(mvsBook[0].wmv);
                printf(" ponder %.4s", (const char *) &dwMoveStr);
              }
              printf("\n");
              fflush(stdout);
              continue;
            }
            pos.UndoMakeMove();
          }
        }

        // "go time ..." -> "go wtime ... btime ..."
        lp = strstr(sz, " time ");
        if (lp != NULL) {
          lpTime = lp + 6;
          // copy text after "time"
          lp2 = strchr(lpTime, ' ');
          if (lp2 == NULL) {
            strcpy(szTime, lpTime + 6);
          } else {
            nLen = lp2 - lpTime;
            strncpy(szTime, lpTime, nLen);
            szTime[nLen] = '\0';
          }
          // copy text after "increment"
          lp2 = strstr(sz, " increment ");
          if (lp2 == NULL) {
            // copy text after "movestogo"
            lp2 = strstr(sz, " movestogo ");
            if (lp2 == NULL) {
              sprintf(lp, " wtime %s btime %s", szTime, szTime);
            } else {
              lpTime = lp2 + 11;
              lp2 = strchr(lpTime, ' ');
              if (lp2 == NULL) {
                strcpy(szInc, lpTime);
              } else {
                nLen = lp2 - lpTime;
                strncpy(szInc, lpTime, nLen);
                szInc[nLen] = '\0';
              }
              sprintf(lp, " wtime %s btime %s movestogo %s", szTime, szTime, szInc);
            }
          } else {
            lpTime = lp2 + 11;
            lp2 = strchr(lpTime, ' ');
            if (lp2 == NULL) {
              strcpy(szInc, lpTime);
            } else {
              nLen = lp2 - lpTime;
              strncpy(szInc, lpTime, nLen);
              szInc[nLen] = '\0';
            }
            sprintf(lp, " wtime %s btime %s winc %s binc %s", szTime, szTime, szInc, szInc);
          }
        }
        // other options such as "go depth ...": unchanged
      }
      pipeEngine.LineOutput(sz);
      // printf("info string GUI->ENG: [%s]\n", sz);
      fflush(stdout);
    }
    Idle();
  }
  pipeConsole.Close();
  pipeEngine.Close();
  return 0;
}