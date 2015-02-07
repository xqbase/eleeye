/*
genmoves.cpp - Source Code for ElephantEye, Part IV

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.0, Last Modified: Nov. 2007
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

#include "../base/base.h"
#include "pregen.h"
#include "position.h"

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上四个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

// 本模块只涉及到"PositionStruct"中的"sdPlayer"、"ucpcSquares"和"ucsqPieces"三个成员，故省略前面的"this->"

// 棋子保护判断
bool PositionStruct::Protected(int sd, int sqSrc, int sqExcept) const {
  // 参数"sqExcept"表示排除保护的棋子(指格子编号)，考虑被牵制子的保护时，需要排除牵制目标子的保护
  int i, sqDst, sqPin, pc, x, y, nSideTag;
  SlideMaskStruct *lpsmsRank, *lpsmsFile;
  // 棋子保护判断包括以下几个步骤：

  __ASSERT_SQUARE(sqSrc);
  nSideTag = SIDE_TAG(sd);
  if (HOME_HALF(sqSrc, sd)) {
    if (IN_FORT(sqSrc)) {

      // 1. 判断受到帅(将)的保护
      sqDst = ucsqPieces[nSideTag + KING_FROM];
      if (sqDst != 0 && sqDst != sqExcept) {
        __ASSERT_SQUARE(sqDst);
        if (KING_SPAN(sqSrc, sqDst)) {
          return true;
        }
      }

      // 2. 判断受到仕(士)的保护
      for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
        sqDst = ucsqPieces[nSideTag + i];
        if (sqDst != 0 && sqDst != sqExcept) {
          __ASSERT_SQUARE(sqDst);
          if (ADVISOR_SPAN(sqSrc, sqDst)) {
            return true;
          }
        }
      }
    }

    // 3. 判断受到相(象)的保护
    for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
      sqDst = ucsqPieces[nSideTag + i];
      if (sqDst != 0 && sqDst != sqExcept) {
        __ASSERT_SQUARE(sqDst);
        if (BISHOP_SPAN(sqSrc, sqDst) && ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0) {
          return true;
        }
      }
    }
  } else {

    // 4. 判断受到过河兵(卒)横向的保护
    for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2) {
      // 如果棋子在边线，那么断言不成立
      // __ASSERT_SQUARE(sqDst);
      if (sqDst != sqExcept) {
        pc = ucpcSquares[sqDst];
        if ((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
          return true;
        }
      }
    }
  }

  // 5. 判断受到兵(卒)纵向的保护
  sqDst = SQUARE_BACKWARD(sqSrc, sd);
  // 如果棋子在底线，那么断言不成立
  // __ASSERT_SQUARE(sqDst);
  if (sqDst != sqExcept) {
    pc = ucpcSquares[sqDst];
    if ((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
      return true;
    }
  }

  // 6. 判断受到马的保护
  for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
    sqDst = ucsqPieces[nSideTag + i];
    if (sqDst != 0 && sqDst != sqExcept) {
      __ASSERT_SQUARE(sqDst);
      sqPin = KNIGHT_PIN(sqDst, sqSrc); // 注意，sqSrc和sqDst是反的！
      if (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
        return true;
      }
    }
  }

  x = FILE_X(sqSrc);
  y = RANK_Y(sqSrc);
  lpsmsRank = RankMaskPtr(x, y);
  lpsmsFile = FileMaskPtr(x, y);

  // 7. 判断受到车的保护，参阅"position.cpp"里的"CheckedBy()"函数
  for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
    sqDst = ucsqPieces[nSideTag + i];
    if (sqDst != 0 && sqDst != sqSrc && sqDst != sqExcept) {
      if (x == FILE_X(sqDst)) {
        if ((lpsmsFile->wRookCap & PreGen.wBitFileMask[sqDst]) != 0) {
          return true;
        }
      } else if (y == RANK_Y(sqDst)) {
        if ((lpsmsRank->wRookCap & PreGen.wBitRankMask[sqDst]) != 0) {
          return true;
        }
      }
    }
  }

  // 8. 判断受到炮的保护，参阅"position.cpp"里的"CheckedBy()"函数
  for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
    sqDst = ucsqPieces[nSideTag + i];
    if (sqDst && sqDst != sqSrc && sqDst != sqExcept) {
      if (x == FILE_X(sqDst)) {
        if ((lpsmsFile->wCannonCap & PreGen.wBitFileMask[sqDst]) != 0) {
          return true;
        }
      } else if (y == RANK_Y(sqDst)) {
        if ((lpsmsRank->wCannonCap & PreGen.wBitRankMask[sqDst]) != 0) {
          return true;
        }
      }
    }
  }
  return false;
}

/* 计算MVV(LVA)值的函数
 *
 * MVV(LVA)指的是：如果被吃子无保护，那么取值MVV，否则取值MVV-LVA。
 * 由于ElephantEye的MVV(LVA)值在计算完毕后再加了1，并且有其它考虑，因此有以下几种含义：
 * a. MVV(LVA)大于1，说明被吃子价值大于攻击子(表面上是赚的)，这种吃子将首先搜索，静态搜索也将考虑这种吃子；
 * b. MVV(LVA)等于1，说明被吃子有一定价值(吃车马炮或吃过河兵卒，即便表面上是亏的，也值得一试)，静态搜索也将考虑这种吃子；
 * c. MVV(LVA)等于0，说明被吃子没有价值，静态搜索将不考虑这种吃子。
 *
 * MVV价值表"SIMPLE_VALUE"是按照帅(将)=5、车=4、马炮=3、兵(卒)=2、仕(士)相(象)=1设定的；
 * LVA价值直接体现在吃子着法生成器中。
 */
int PositionStruct::MvvLva(int sqDst, int pcCaptured, int nLva) const {
  int nMvv, nLvaAdjust;
  nMvv = SIMPLE_VALUE(pcCaptured);
  nLvaAdjust = (Protected(OPP_SIDE(sdPlayer), sqDst) ? nLva : 0);
  if (nMvv >= nLvaAdjust) {
    return nMvv - nLvaAdjust + 1;
  } else {
    return (nMvv >= 3 || HOME_HALF(sqDst, sdPlayer)) ? 1 : 0;
  }
}

// 吃子着法生成器，按MVV(LVA)设定分值
int PositionStruct::GenCapMoves(MoveStruct *lpmvs) const {
  int i, sqSrc, sqDst, pcCaptured;
  int x, y, nSideTag, nOppSideTag;
  bool bCanPromote;
  SlideMoveStruct *lpsmv;
  uint8_t *lpucsqDst, *lpucsqPin;
  MoveStruct *lpmvsCurr;
  // 生成吃子着法的过程包括以下几个步骤：

  lpmvsCurr = lpmvs;
  nSideTag = SIDE_TAG(sdPlayer);
  nOppSideTag = OPP_SIDE_TAG(sdPlayer);
  bCanPromote = PreEval.bPromotion && CanPromote();

  // 1. 生成帅(将)的着法
  sqSrc = ucsqPieces[nSideTag + KING_FROM];
  if (sqSrc != 0) {
    __ASSERT_SQUARE(sqSrc);
    lpucsqDst = PreGen.ucsqKingMoves[sqSrc];
    sqDst = *lpucsqDst;
    while (sqDst != 0) {
      __ASSERT_SQUARE(sqDst);
      // 找到一个着法后，首先判断吃到的棋子是否是对方棋子，技巧是利用"nOppSideTag"的标志(16和32颠倒)，
      // 如果是对方棋子，则保存MVV(LVA)值，即如果被吃子无保护，则只记MVV，否则记MVV-LVA(如果MVV>LVA的话)。
      pcCaptured = ucpcSquares[sqDst];
      if ((pcCaptured & nOppSideTag) != 0) {
        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
        lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
        lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 5); // 帅(将)的价值是5
        lpmvsCurr ++;
      }
      lpucsqDst ++;
      sqDst = *lpucsqDst;
    }
  }

  // 2. 生成仕(士)的着法
  for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqAdvisorMoves[sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 1); // 仕(士)的价值是1
          lpmvsCurr ++;
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
      }
      if (bCanPromote && CAN_PROMOTE(sqSrc)) {
        lpmvsCurr->wmv = MOVE(sqSrc, sqSrc);
        lpmvsCurr->wvl = 0;
        lpmvsCurr ++;
      }
    }
  }

  // 3. 生成相(象)的着法
  for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqBishopMoves[sqSrc];
      lpucsqPin = PreGen.ucsqBishopPins[sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        if (ucpcSquares[*lpucsqPin] == 0) {
          pcCaptured = ucpcSquares[sqDst];
          if ((pcCaptured & nOppSideTag) != 0) {
            __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
            lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
            lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 1); // 相(象)的价值是1
            lpmvsCurr ++;
          }
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
        lpucsqPin ++;
      }
      if (bCanPromote && CAN_PROMOTE(sqSrc)) {
        lpmvsCurr->wmv = MOVE(sqSrc, sqSrc);
        lpmvsCurr->wvl = 0;
        lpmvsCurr ++;
      }
    }
  }

  // 4. 生成马的着法
  for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqKnightMoves[sqSrc];
      lpucsqPin = PreGen.ucsqKnightPins[sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        if (ucpcSquares[*lpucsqPin] == 0) {
          pcCaptured = ucpcSquares[sqDst];
          if ((pcCaptured & nOppSideTag) != 0) {
            __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
            lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
            lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 马的价值是3
            lpmvsCurr ++;
          }
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
        lpucsqPin ++;
      }
    }
  }

  // 5. 生成车的着法
  for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      x = FILE_X(sqSrc);
      y = RANK_Y(sqSrc);

      lpsmv = RankMovePtr(x, y);
      sqDst = lpsmv->ucRookCap[0] + RANK_DISP(y);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
          lpmvsCurr ++;
        }
      }
      sqDst = lpsmv->ucRookCap[1] + RANK_DISP(y);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
          lpmvsCurr ++;
        }
      }

      lpsmv = FileMovePtr(x, y);
      sqDst = lpsmv->ucRookCap[0] + FILE_DISP(x);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
          lpmvsCurr ++;
        }
      }
      sqDst = lpsmv->ucRookCap[1] + FILE_DISP(x);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 4); // 车的价值是4
          lpmvsCurr ++;
        }
      }
    }
  }

  // 6. 生成炮的着法
  for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      x = FILE_X(sqSrc);
      y = RANK_Y(sqSrc);

      lpsmv = RankMovePtr(x, y);
      sqDst = lpsmv->ucCannonCap[0] + RANK_DISP(y);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
          lpmvsCurr ++;
        }
      }
      sqDst = lpsmv->ucCannonCap[1] + RANK_DISP(y);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
          lpmvsCurr ++;
        }
      }

      lpsmv = FileMovePtr(x, y);
      sqDst = lpsmv->ucCannonCap[0] + FILE_DISP(x);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
          lpmvsCurr ++;
        }
      }
      sqDst = lpsmv->ucCannonCap[1] + FILE_DISP(x);
      __ASSERT_SQUARE(sqDst);
      if (sqDst != sqSrc) {
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 3); // 炮的价值是3
          lpmvsCurr ++;
        }
      }
    }
  }

  // 7. 生成兵(卒)的着法
  for (i = PAWN_FROM; i <= PAWN_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqPawnMoves[sdPlayer][sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        pcCaptured = ucpcSquares[sqDst];
        if ((pcCaptured & nOppSideTag) != 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->wmv = MOVE(sqSrc, sqDst);
          lpmvsCurr->wvl = MvvLva(sqDst, pcCaptured, 2); // 兵(卒)的价值是2
          lpmvsCurr ++;
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
      }
    }
  }
  return lpmvsCurr - lpmvs;
}

// 不吃子着法生成器
int PositionStruct::GenNonCapMoves(MoveStruct *lpmvs) const {
  int i, sqSrc, sqDst, x, y, nSideTag;
  SlideMoveStruct *lpsmv;
  uint8_t *lpucsqDst, *lpucsqPin;
  MoveStruct *lpmvsCurr;
  // 生成不吃子着法的过程包括以下几个步骤：

  lpmvsCurr = lpmvs;
  nSideTag = SIDE_TAG(sdPlayer);

  // 1. 生成帅(将)的着法
  sqSrc = ucsqPieces[nSideTag + KING_FROM];
  if (sqSrc != 0) {
    __ASSERT_SQUARE(sqSrc);
    lpucsqDst = PreGen.ucsqKingMoves[sqSrc];
    sqDst = *lpucsqDst;
    while (sqDst != 0) {
      __ASSERT_SQUARE(sqDst);
      // 找到一个着法后，首先判断是否吃到棋子
      if (ucpcSquares[sqDst] == 0) {
        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
        lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
        lpmvsCurr ++;
      }
      lpucsqDst ++;
      sqDst = *lpucsqDst;
    }
  }

  // 2. 生成仕(士)的着法
  for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqAdvisorMoves[sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        if (ucpcSquares[sqDst] == 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
          lpmvsCurr ++;
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
      }
    }
  }

  // 3. 生成相(象)的着法
  for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqBishopMoves[sqSrc];
      lpucsqPin = PreGen.ucsqBishopPins[sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        if (ucpcSquares[*lpucsqPin] == 0 && ucpcSquares[sqDst] == 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
          lpmvsCurr ++;
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
        lpucsqPin ++;
      }
    }
  }

  // 4. 生成马的着法
  for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqKnightMoves[sqSrc];
      lpucsqPin = PreGen.ucsqKnightPins[sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        if (ucpcSquares[*lpucsqPin] == 0 && ucpcSquares[sqDst] == 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
          lpmvsCurr ++;
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
        lpucsqPin ++;
      }
    }
  }

  // 5. 生成车和炮的着法，没有必要判断是否吃到本方棋子
  for (i = ROOK_FROM; i <= CANNON_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      x = FILE_X(sqSrc);
      y = RANK_Y(sqSrc);

      lpsmv = RankMovePtr(x, y);
      sqDst = lpsmv->ucNonCap[0] + RANK_DISP(y);
      __ASSERT_SQUARE(sqDst);
      while (sqDst != sqSrc) {
        __ASSERT(ucpcSquares[sqDst] == 0);
        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
        lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
        lpmvsCurr ++;
        sqDst --;
      }
      sqDst = lpsmv->ucNonCap[1] + RANK_DISP(y);
      __ASSERT_SQUARE(sqDst);
      while (sqDst != sqSrc) {
        __ASSERT(ucpcSquares[sqDst] == 0);
        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
        lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
        lpmvsCurr ++;
        sqDst ++;
      }

      lpsmv = FileMovePtr(x, y);
      sqDst = lpsmv->ucNonCap[0] + FILE_DISP(x);
      __ASSERT_SQUARE(sqDst);
      while (sqDst != sqSrc) {
        __ASSERT(ucpcSquares[sqDst] == 0);
        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
        lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
        lpmvsCurr ++;
        sqDst -= 16;
      }
      sqDst = lpsmv->ucNonCap[1] + FILE_DISP(x);
      __ASSERT_SQUARE(sqDst);
      while (sqDst != sqSrc) {
        __ASSERT(ucpcSquares[sqDst] == 0);
        __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
        lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
        lpmvsCurr ++;
        sqDst += 16;
      }
    }
  }

  // 6. 生成兵(卒)的着法
  for (i = PAWN_FROM; i <= PAWN_TO; i ++) {
    sqSrc = ucsqPieces[nSideTag + i];
    if (sqSrc != 0) {
      __ASSERT_SQUARE(sqSrc);
      lpucsqDst = PreGen.ucsqPawnMoves[sdPlayer][sqSrc];
      sqDst = *lpucsqDst;
      while (sqDst != 0) {
        __ASSERT_SQUARE(sqDst);
        if (ucpcSquares[sqDst] == 0) {
          __ASSERT(LegalMove(MOVE(sqSrc, sqDst)));
          lpmvsCurr->dwmv = MOVE(sqSrc, sqDst);
          lpmvsCurr ++;
        }
        lpucsqDst ++;
        sqDst = *lpucsqDst;
      }
    }
  }
  return lpmvsCurr - lpmvs;
}

// “捉”的检测
int PositionStruct::ChasedBy(int mv) const {
  int i, nSideTag, pcMoved, pcCaptured;
  int sqSrc, sqDst, x, y;
  uint8_t *lpucsqDst, *lpucsqPin;
  SlideMoveStruct *lpsmv;

  sqSrc = DST(mv);
  pcMoved = this->ucpcSquares[sqSrc];
  nSideTag = SIDE_TAG(this->sdPlayer);
  __ASSERT_SQUARE(sqSrc);
  __ASSERT_PIECE(pcMoved);
  __ASSERT_BOUND(0, pcMoved - OPP_SIDE_TAG(this->sdPlayer), 15);

  // “捉”的判断包括以下几部分内容：
  switch (pcMoved - OPP_SIDE_TAG(this->sdPlayer)) {

  // 1. 走了马，判断是否捉车或捉有根的炮兵(卒)
  case KNIGHT_FROM:
  case KNIGHT_TO:
    // 逐一检测马踩的八个位置
    lpucsqDst = PreGen.ucsqKnightMoves[sqSrc];
    lpucsqPin = PreGen.ucsqKnightPins[sqSrc];
    sqDst = *lpucsqDst;
    while (sqDst != 0) {
      __ASSERT_SQUARE(sqDst);
      if (ucpcSquares[*lpucsqPin] == 0) {
        pcCaptured = this->ucpcSquares[sqDst];
        if ((pcCaptured & nSideTag) != 0) {
          pcCaptured -= nSideTag;
          __ASSERT_BOUND(0, pcCaptured, 15);
          // 技巧：优化兵种判断的分枝
          if (pcCaptured <= ROOK_TO) {
            // 马捉仕(士)、相(象)和马的情况不予考虑
            if (pcCaptured >= ROOK_FROM) {
              // 马捉到了车
              return pcCaptured;
            }
          } else {
            if (pcCaptured <= CANNON_TO) {
              // 马捉到了炮，要判断炮是否受保护
              if (!Protected(this->sdPlayer, sqDst)) {
                return pcCaptured;
              }
            } else {
              // 马捉到了兵(卒)，要判断兵(卒)是否过河并受保护
              if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                return pcCaptured;
              }
            }
          }
        }
      }
      lpucsqDst ++;
      sqDst = *lpucsqDst;
      lpucsqPin ++;
    }
    break;

  // 2. 走了车，判断是否捉有根的马炮兵(卒)
  case ROOK_FROM:
  case ROOK_TO:
    x = FILE_X(sqSrc);
    y = RANK_Y(sqSrc);
    if (((SRC(mv) ^ sqSrc) & 0xf) == 0) {
      // 如果车纵向移动了，则判断车横向吃到的子
      lpsmv = RankMovePtr(x, y);
      for (i = 0; i < 2; i ++) {
        sqDst = lpsmv->ucRookCap[i] + RANK_DISP(y);
        __ASSERT_SQUARE(sqDst);
        if (sqDst != sqSrc) {
          pcCaptured = this->ucpcSquares[sqDst];
          if ((pcCaptured & nSideTag) != 0) {
            pcCaptured -= nSideTag;
            __ASSERT_BOUND(0, pcCaptured, 15);
            // 技巧：优化兵种判断的分枝
            if (pcCaptured <= ROOK_TO) {
              // 车捉仕(士)、相(象)的情况不予考虑
              if (pcCaptured >= KNIGHT_FROM) {
                if (pcCaptured <= KNIGHT_TO) {
                  // 车捉到了马，要判断马是否受保护
                  if (!Protected(this->sdPlayer, sqDst)) {
                    return pcCaptured;
                  }
                }
                // 车捉车的情况不予考虑
              }
            } else {
              if (pcCaptured <= CANNON_TO) {
                // 车捉到了炮，要判断炮是否受保护
                if (!Protected(this->sdPlayer, sqDst)) {
                  return pcCaptured;
                }
              } else {
                // 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                  return pcCaptured;
                }
              }
            }
          }
        }
      }
    } else {
      // 如果车横向移动了，则判断车纵向吃到的子
      lpsmv = FileMovePtr(x, y);
      for (i = 0; i < 2; i ++) {
        sqDst = lpsmv->ucRookCap[i] + FILE_DISP(x);
        __ASSERT_SQUARE(sqDst);
        if (sqDst != sqSrc) {
          pcCaptured = this->ucpcSquares[sqDst];
          if ((pcCaptured & nSideTag) != 0) {
            pcCaptured -= nSideTag;
            __ASSERT_BOUND(0, pcCaptured, 15);
            // 技巧：优化兵种判断的分枝
            if (pcCaptured <= ROOK_TO) {
              // 车捉仕(士)、相(象)的情况不予考虑
              if (pcCaptured >= KNIGHT_FROM) {
                if (pcCaptured <= KNIGHT_TO) {
                  // 车捉到了马，要判断马是否受保护
                  if (!Protected(this->sdPlayer, sqDst)) {
                    return pcCaptured;
                  }
                }
                // 车捉车的情况不予考虑
              }
            } else {
              if (pcCaptured <= CANNON_TO) {
                // 车捉到了炮，要判断炮是否受保护
                if (!Protected(this->sdPlayer, sqDst)) {
                  return pcCaptured;
                }
              } else {
                // 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                  return pcCaptured;
                }
              }
            }
          }
        }
      }
    }
    break;

  // 3. 走了炮，判断是否捉车或捉有根的马兵(卒)
  case CANNON_FROM:
  case CANNON_TO:
    x = FILE_X(sqSrc);
    y = RANK_Y(sqSrc);
    if (((SRC(mv) ^ sqSrc) & 0xf) == 0) {
      // 如果炮纵向移动了，则判断炮横向吃到的子
      lpsmv = RankMovePtr(x, y);
      for (i = 0; i < 2; i ++) {
        sqDst = lpsmv->ucCannonCap[i] + RANK_DISP(y);
        __ASSERT_SQUARE(sqDst);
        if (sqDst != sqSrc) {
          pcCaptured = this->ucpcSquares[sqDst];
          if ((pcCaptured & nSideTag) != 0) {
            pcCaptured -= nSideTag;
            __ASSERT_BOUND(0, pcCaptured, 15);
            // 技巧：优化兵种判断的分枝
            if (pcCaptured <= ROOK_TO) {
              // 炮捉仕(士)、相(象)的情况不予考虑
              if (pcCaptured >= KNIGHT_FROM) {
                if (pcCaptured <= KNIGHT_TO) {
                  // 炮捉到了马，要判断马是否受保护
                  if (!Protected(this->sdPlayer, sqDst)) {
                    return pcCaptured;
                  }
                } else {
                  // 炮捉到了车
                  return pcCaptured;
                }
              }
            } else {
              // 炮捉炮的情况不予考虑
              if (pcCaptured >= PAWN_FROM) {
                // 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                  return pcCaptured;
                }
              }
            }
          }
        }
      }
    } else {
      // 如果炮横向移动了，则判断炮纵向吃到的子
      lpsmv = FileMovePtr(x, y);
      for (i = 0; i < 2; i ++) {
        sqDst = lpsmv->ucCannonCap[i] + FILE_DISP(x);
        __ASSERT_SQUARE(sqDst);
        if (sqDst != sqSrc) {
          pcCaptured = this->ucpcSquares[sqDst];
          if ((pcCaptured & nSideTag) != 0) {
            pcCaptured -= nSideTag;
            __ASSERT_BOUND(0, pcCaptured, 15);
            // 技巧：优化兵种判断的分枝
            if (pcCaptured <= ROOK_TO) {
              // 炮捉仕(士)、相(象)的情况不予考虑
              if (pcCaptured >= KNIGHT_FROM) {
                if (pcCaptured <= KNIGHT_TO) {
                  // 炮捉到了马，要判断马是否受保护
                  if (!Protected(this->sdPlayer, sqDst)) {
                    return pcCaptured;
                  }
                } else {
                  // 炮捉到了车
                  return pcCaptured;
                }
              }
            } else {
              // 炮捉炮的情况不予考虑
              if (pcCaptured >= PAWN_FROM) {
                // 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
                if (AWAY_HALF(sqDst, sdPlayer) && !Protected(this->sdPlayer, sqDst)) {
                  return pcCaptured;
                }
              }
            }
          }
        }
      }
    }
    break;
  }

  return 0;
}
