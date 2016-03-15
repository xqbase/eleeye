/*
position.h/position.cpp - Source Code for ElephantEye, Part III

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
 * (注：以上五个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

// 本模块涉及多个"PositionStruct"的成员，用"this->"标记出来以方便辨认

// 起始局面的FEN串
const char *const cszStartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";

// 棋子类型对应的棋子符号
const char *const cszPieceBytes = "KABNRCP";

/* 棋子序号对应的棋子类型
 *
 * ElephantEye的棋子序号从0到47，其中0到15不用，16到31表示红子，32到47表示黑子。
 * 每方的棋子顺序依次是：帅仕仕相相马马车车炮炮兵兵兵兵兵(将士士象象马马车车炮炮卒卒卒卒卒)
 * 提示：判断棋子是红子用"pc < 32"，黑子用"pc >= 32"
 */
const int cnPieceTypes[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
};

// 棋子的简单分值，只在简单比较时作参考
const int cnSimpleValues[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
  5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
};

// 该数组很方便地实现了坐标的镜像(左右对称)
const uint8_t cucsqMirrorTab[256] = {
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0, 0, 0, 0,
  0, 0, 0, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0, 0, 0, 0,
  0, 0, 0, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54, 0x53, 0, 0, 0, 0,
  0, 0, 0, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0, 0, 0, 0,
  0, 0, 0, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0, 0, 0, 0,
  0, 0, 0, 0x8b, 0x8a, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0, 0, 0, 0,
  0, 0, 0, 0x9b, 0x9a, 0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0, 0, 0, 0,
  0, 0, 0, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0, 0, 0, 0,
  0, 0, 0, 0xbb, 0xba, 0xb9, 0xb8, 0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 0, 0, 0, 0,
  0, 0, 0, 0xcb, 0xca, 0xc9, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
};

// FEN串中棋子标识，注意这个函数只能识别大写字母，因此用小写字母时，首先必须转换为大写
int FenPiece(int nArg) {
  switch (nArg) {
  case 'K':
    return 0;
  case 'A':
    return 1;
  case 'B':
  case 'E':
    return 2;
  case 'N':
  case 'H':
    return 3;
  case 'R':
    return 4;
  case 'C':
    return 5;
  case 'P':
    return 6;
  default:
    return 7;
  }
}

// 以下是一些棋盘处理过程

// 棋盘上增加棋子
void PositionStruct::AddPiece(int sq, int pc, bool bDel) {
  int pt;

  __ASSERT_SQUARE(sq);
  __ASSERT_PIECE(pc);
  if (bDel) {
    this->ucpcSquares[sq] = 0;
    this->ucsqPieces[pc] = 0;
  } else {
    this->ucpcSquares[sq] = pc;
    this->ucsqPieces[pc] = sq;
  }
  this->wBitRanks[RANK_Y(sq)] ^= PreGen.wBitRankMask[sq];
  this->wBitFiles[FILE_X(sq)] ^= PreGen.wBitFileMask[sq];
  __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sq)]);
  __ASSERT_BITFILE(this->wBitRanks[FILE_X(sq)]);
  this->dwBitPiece ^= BIT_PIECE(pc);
  pt = PIECE_TYPE(pc);
  if (pc < 32) {
    if (bDel) {
      this->vlWhite -= PreEval.ucvlWhitePieces[pt][sq];
    } else {
      this->vlWhite += PreEval.ucvlWhitePieces[pt][sq];
    }
  } else {
    if (bDel) {
      this->vlBlack -= PreEval.ucvlBlackPieces[pt][sq];
    } else {
      this->vlBlack += PreEval.ucvlBlackPieces[pt][sq];
    }
    pt += 7;
  }
  __ASSERT_BOUND(0, pt, 13);
  this->zobr.Xor(PreGen.zobrTable[pt][sq]);
}

// 移动棋子
int PositionStruct::MovePiece(int mv) {
  int sqSrc, sqDst, pcMoved, pcCaptured, pt;
  uint8_t *lpucvl;
  // 移动棋子包括以下几个步骤：

  // 1. 得到移动的棋子序号和被吃的棋子序号；
  sqSrc = SRC(mv);
  sqDst = DST(mv);
  pcMoved = this->ucpcSquares[sqSrc];
  __ASSERT_SQUARE(sqSrc);
  __ASSERT_SQUARE(sqDst);
  __ASSERT_PIECE(pcMoved);
  pcCaptured = this->ucpcSquares[sqDst];
  if (pcCaptured == 0) {

    // 2. 如果没有被吃的棋子，那么更新目标格的位行和位列。
    //    换句话说，有被吃的棋子，目标格的位行和位列就不必更新了。
    this->wBitRanks[RANK_Y(sqDst)] ^= PreGen.wBitRankMask[sqDst];
    this->wBitFiles[FILE_X(sqDst)] ^= PreGen.wBitFileMask[sqDst];
    __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqDst)]);
    __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqDst)]);
  } else {

    __ASSERT_PIECE(pcCaptured);
    // 3. 如果有被吃的棋子，那么更新棋子位，从"ucsqPieces"数组中清除被吃棋子
    //    同时更新子力价值、位行位列、Zobrist键值和校验锁
    this->ucsqPieces[pcCaptured] = 0;
    this->dwBitPiece ^= BIT_PIECE(pcCaptured);
    pt = PIECE_TYPE(pcCaptured);
    if (pcCaptured < 32) {
      this->vlWhite -= PreEval.ucvlWhitePieces[pt][sqDst];
    } else {
      this->vlBlack -= PreEval.ucvlBlackPieces[pt][sqDst];
      pt += 7;
    }
    __ASSERT_BOUND(0, pt, 13);
    this->zobr.Xor(PreGen.zobrTable[pt][sqDst]);
  }

  // 4. 从"ucpcSquares"和"ucsqPieces"数组中移动棋子，注意“格子-棋子联系数组”移动棋子的方法
  //    同时更新位行、位列、子力价值、位行位列、Zobrist键值和校验锁
  this->ucpcSquares[sqSrc] = 0;
  this->ucpcSquares[sqDst] = pcMoved;
  this->ucsqPieces[pcMoved] = sqDst;
  this->wBitRanks[RANK_Y(sqSrc)] ^= PreGen.wBitRankMask[sqSrc];
  this->wBitFiles[FILE_X(sqSrc)] ^= PreGen.wBitFileMask[sqSrc];
  __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqSrc)]);
  __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqSrc)]);
  pt = PIECE_TYPE(pcMoved);
  if (pcMoved < 32) {
    lpucvl = PreEval.ucvlWhitePieces[pt];
    this->vlWhite += lpucvl[sqDst] - lpucvl[sqSrc];
  } else {
    lpucvl = PreEval.ucvlBlackPieces[pt];
    this->vlBlack += lpucvl[sqDst] - lpucvl[sqSrc];
    pt += 7;
  }
  __ASSERT_BOUND(0, pt, 13);
  this->zobr.Xor(PreGen.zobrTable[pt][sqDst], PreGen.zobrTable[pt][sqSrc]);
  return pcCaptured;
}

// 撤消移动棋子
void PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
  int sqSrc, sqDst, pcMoved;
  sqSrc = SRC(mv);
  sqDst = DST(mv);
  pcMoved = this->ucpcSquares[sqDst];
  __ASSERT_SQUARE(sqSrc);
  __ASSERT_SQUARE(sqDst);
  __ASSERT_PIECE(pcMoved);
  this->ucpcSquares[sqSrc] = pcMoved;
  this->ucsqPieces[pcMoved] = sqSrc;
  this->wBitRanks[RANK_Y(sqSrc)] ^= PreGen.wBitRankMask[sqSrc];
  this->wBitFiles[FILE_X(sqSrc)] ^= PreGen.wBitFileMask[sqSrc];
  __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqSrc)]);
  __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqSrc)]);
  if (pcCaptured > 0) {
    __ASSERT_PIECE(pcCaptured);
    this->ucpcSquares[sqDst] = pcCaptured;
    this->ucsqPieces[pcCaptured] = sqDst;
    this->dwBitPiece ^= BIT_PIECE(pcCaptured);
  } else {
    this->ucpcSquares[sqDst] = 0;
    this->wBitRanks[RANK_Y(sqDst)] ^= PreGen.wBitRankMask[sqDst];
    this->wBitFiles[FILE_X(sqDst)] ^= PreGen.wBitFileMask[sqDst];
    __ASSERT_BITRANK(this->wBitRanks[RANK_Y(sqDst)]);
    __ASSERT_BITFILE(this->wBitRanks[FILE_X(sqDst)]);
  }
}

// 升变
int PositionStruct::Promote(int sq) {
  int pcCaptured, pcPromoted, pt;
  // 升变包括以下几个步骤：

  // 1. 得到升变前棋子的序号；
  __ASSERT_SQUARE(sq);
  __ASSERT(CanPromote());
  __ASSERT(CAN_PROMOTE(sq));
  pcCaptured = this->ucpcSquares[sq];
  __ASSERT_PIECE(pcCaptured);
  pcPromoted = SIDE_TAG(this->sdPlayer) + Bsf(~this->wBitPiece[this->sdPlayer] & PAWN_BITPIECE);
  __ASSERT_PIECE(pcPromoted);
  __ASSERT(this->ucsqPieces[pcPromoted] == 0);

  // 2. 去掉升变前棋子，同时更新子力价值、Zobrist键值和校验锁
  this->dwBitPiece ^= BIT_PIECE(pcPromoted) ^ BIT_PIECE(pcCaptured);
  this->ucsqPieces[pcCaptured] = 0;
  pt = PIECE_TYPE(pcCaptured);
  if (pcCaptured < 32) {
    this->vlWhite -= PreEval.ucvlWhitePieces[pt][sq];
  } else {
    this->vlBlack -= PreEval.ucvlBlackPieces[pt][sq];
    pt += 7;
  }
  __ASSERT_BOUND(0, pt, 13);
  this->zobr.Xor(PreGen.zobrTable[pt][sq]);

  // 3. 加上升变后棋子，同时更新子力价值、Zobrist键值和校验锁
  this->ucpcSquares[sq] = pcPromoted;
  this->ucsqPieces[pcPromoted] = sq;
  pt = PIECE_TYPE(pcPromoted);
  if (pcPromoted < 32) {
    this->vlWhite += PreEval.ucvlWhitePieces[pt][sq];
  } else {
    this->vlBlack += PreEval.ucvlBlackPieces[pt][sq];
    pt += 7;
  }
  __ASSERT_BOUND(0, pt, 13);
  this->zobr.Xor(PreGen.zobrTable[pt][sq]);
  return pcCaptured;
}

// 撤消升变
void PositionStruct::UndoPromote(int sq, int pcCaptured) {
  int pcPromoted;
  __ASSERT_SQUARE(sq);
  __ASSERT_PIECE(pcCaptured);
  pcPromoted = this->ucpcSquares[sq];
  __ASSERT(PIECE_TYPE(pcPromoted) == 6);
  this->ucsqPieces[pcPromoted] = 0;
  this->ucpcSquares[sq] = pcCaptured;
  this->ucsqPieces[pcCaptured] = sq;
  this->dwBitPiece ^= BIT_PIECE(pcPromoted) ^ BIT_PIECE(pcCaptured);
}

// 以上是一些棋盘处理过程

// 以下是一些着法处理过程

// 执行一个着法
bool PositionStruct::MakeMove(int mv) {
  int sq, pcCaptured;
  uint32_t dwOldZobristKey;
  RollbackStruct *lprbs;

  // 如果达到最大着法数，那么判定为非法着法
  if (this->nMoveNum == MAX_MOVE_NUM) {
    return false;
  }
  __ASSERT(this->nMoveNum < MAX_MOVE_NUM);
  // 执行一个着法要包括以下几个步骤：

  // 1. 保存原来的Zobrist键值
  dwOldZobristKey = this->zobr.dwKey;
  SaveStatus();

  // 2. 移动棋子，记住吃掉的子(如果有的话)
  sq = SRC(mv);
  if (sq == DST(mv)) {
    pcCaptured = Promote(sq);
  } else {
    pcCaptured = MovePiece(mv);

    // 3. 如果移动后被将军了，那么着法是非法的，撤消该着法
    if (CheckedBy(CHECK_LAZY) > 0) {
      UndoMovePiece(mv, pcCaptured);
      Rollback();
      return false;
    }
  }

  // 4. 交换走子方
  ChangeSide();

  // 5. 把原来的Zobrist键值记录到检测重复的迷你置换表中
  if (this->ucRepHash[dwOldZobristKey & REP_HASH_MASK] == 0) {
    this->ucRepHash[dwOldZobristKey & REP_HASH_MASK] = this->nMoveNum;
  }

  // 6. 把着法保存到历史着法列表中，并记住吃掉的子和将军状态
  lprbs = this->rbsList + this->nMoveNum;
  lprbs->mvs.wmv = mv;
  lprbs->mvs.ChkChs = CheckedBy();

  // 7. 设置和棋着法数(将军和应将不计入)
  if (pcCaptured == 0) {
    if (lprbs->mvs.ChkChs == 0) {
      lprbs->mvs.ChkChs = -ChasedBy(mv);
    }
    if (LastMove().CptDrw == -100) {
      lprbs->mvs.CptDrw = -100;
    } else {
      lprbs->mvs.CptDrw = MIN((int) LastMove().CptDrw, 0) - (lprbs->mvs.ChkChs > 0 || LastMove().ChkChs > 0 ? 0 : 1);
    }
    __ASSERT_BOUND(-100, lprbs->mvs.CptDrw, 0);
  } else {
    lprbs->mvs.CptDrw = pcCaptured;
    __ASSERT_PIECE(pcCaptured);
  }
  this->nMoveNum ++;
  this->nDistance ++;

  return true;
}

// 撤消一个着法
void PositionStruct::UndoMakeMove(void) {
  int sq;
  RollbackStruct *lprbs;
  this->nMoveNum --;
  this->nDistance --;
  lprbs = this->rbsList + this->nMoveNum;
  sq = SRC(lprbs->mvs.wmv);
  if (sq == DST(lprbs->mvs.wmv)) {
    __ASSERT_BOUND(ADVISOR_TYPE, PIECE_TYPE(lprbs->mvs.CptDrw), BISHOP_TYPE);
    UndoPromote(sq, lprbs->mvs.CptDrw);
  } else {
    UndoMovePiece(lprbs->mvs.wmv, lprbs->mvs.CptDrw);
  }
  this->sdPlayer = OPP_SIDE(this->sdPlayer);
  Rollback();
  if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == this->nMoveNum) {
    this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] = 0;
  }
  __ASSERT(this->nMoveNum > 0);
}

// 执行一个空着
void PositionStruct::NullMove(void) {
  __ASSERT(this->nMoveNum < MAX_MOVE_NUM);
  if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == 0) {
    this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] = this->nMoveNum;
  }
  SaveStatus();
  ChangeSide();
  this->rbsList[nMoveNum].mvs.dwmv = 0; // wmv, Chk, CptDrw, ChkChs = 0
  this->nMoveNum ++;
  this->nDistance ++;
}

// 撤消一个空着
void PositionStruct::UndoNullMove(void) {
  this->nMoveNum --;
  this->nDistance --;
  this->sdPlayer = OPP_SIDE(this->sdPlayer);
  Rollback();
  if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == this->nMoveNum) {
    this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] = 0;
  }
  __ASSERT(this->nMoveNum > 0);
}

// 以上是一些着法处理过程

// 以下是一些局面处理过程

// FEN串识别
void PositionStruct::FromFen(const char *szFen) {
  int i, j, k;
  int pcWhite[7];
  int pcBlack[7];
  const char *lpFen;
  // FEN串的识别包括以下几个步骤：
  // 1. 初始化，清空棋盘
  pcWhite[0] = SIDE_TAG(0) + KING_FROM;
  pcWhite[1] = SIDE_TAG(0) + ADVISOR_FROM;
  pcWhite[2] = SIDE_TAG(0) + BISHOP_FROM;
  pcWhite[3] = SIDE_TAG(0) + KNIGHT_FROM;
  pcWhite[4] = SIDE_TAG(0) + ROOK_FROM;
  pcWhite[5] = SIDE_TAG(0) + CANNON_FROM;
  pcWhite[6] = SIDE_TAG(0) + PAWN_FROM;
  for (i = 0; i < 7; i ++) {
    pcBlack[i] = pcWhite[i] + 16;
  }
  /* 数组"pcWhite[7]"和"pcBlack[7]"分别代表红方和黑方每个兵种即将占有的序号，
   * 以"pcWhite[7]"为例，由于棋子16到31依次代表“帅仕仕相相马马车车炮炮兵兵兵兵兵”，
   * 所以最初应该是"pcWhite[7] = {16, 17, 19, 21, 23, 25, 27}"，每添加一个棋子，该项就增加1，
   * 这种做法允许添加多余的棋子(例如添加第二个帅，就变成仕了)，但添加前要做边界检测
   */
  ClearBoard();
  lpFen = szFen;
  if (*lpFen == '\0') {
    SetIrrev();
    return;
  }
  // 2. 读取棋盘上的棋子
  i = RANK_TOP;
  j = FILE_LEFT;
  while (*lpFen != ' ') {
    if (*lpFen == '/') {
      j = FILE_LEFT;
      i ++;
      if (i > RANK_BOTTOM) {
        break;
      }
    } else if (*lpFen >= '1' && *lpFen <= '9') {
      j += (*lpFen - '0');
    } else if (*lpFen >= 'A' && *lpFen <= 'Z') {
      if (j <= FILE_RIGHT) {
        k = FenPiece(*lpFen);
        if (k < 7) {
          if (pcWhite[k] < 32) {
            if (this->ucsqPieces[pcWhite[k]] == 0) {
              AddPiece(COORD_XY(j, i), pcWhite[k]);
              pcWhite[k] ++;
            }
          }
        }
        j ++;
      }
    } else if (*lpFen >= 'a' && *lpFen <= 'z') {
      if (j <= FILE_RIGHT) {
        k = FenPiece(*lpFen + 'A' - 'a');
        if (k < 7) {
          if (pcBlack[k] < 48) {
            if (this->ucsqPieces[pcBlack[k]] == 0) {
              AddPiece(COORD_XY(j, i), pcBlack[k]);
              pcBlack[k] ++;
            }
          }
        }
        j ++;
      }
    }
    lpFen ++;
    if (*lpFen == '\0') {
      SetIrrev();
      return;
    }
  }
  lpFen ++;
  // 3. 确定轮到哪方走
  if (*lpFen == 'b') {
    ChangeSide();
  }
  // 4. 把局面设成“不可逆”
  SetIrrev();
}

// 生成FEN串
void PositionStruct::ToFen(char *szFen) const {
  int i, j, k, pc;
  char *lpFen;

  lpFen = szFen;
  for (i = RANK_TOP; i <= RANK_BOTTOM; i ++) {
    k = 0;
    for (j = FILE_LEFT; j <= FILE_RIGHT; j ++) {
      pc = this->ucpcSquares[COORD_XY(j, i)];
      if (pc != 0) {
        if (k > 0) {
          *lpFen = k + '0';
          lpFen ++;
          k = 0;
        }
        *lpFen = PIECE_BYTE(PIECE_TYPE(pc)) + (pc < 32 ? 0 : 'a' - 'A');
        lpFen ++;
      } else {
        k ++;
      }
    }
    if (k > 0) {
      *lpFen = k + '0';
      lpFen ++;
    }
    *lpFen = '/';
    lpFen ++;
  }
  *(lpFen - 1) = ' '; // 把最后一个'/'替换成' '
  *lpFen = (this->sdPlayer == 0 ? 'w' : 'b');
  lpFen ++;
  *lpFen = '\0';
}

// 局面镜像
void PositionStruct::Mirror(void) {
  int i, sq, nMoveNumSave;
  uint16_t wmvList[MAX_MOVE_NUM];
  uint8_t ucsqList[32];
  // 局面镜像需要按以下步骤依次进行：

  // 1. 记录所有历史着法
  nMoveNumSave = this->nMoveNum;
  for (i = 1; i < nMoveNumSave; i ++) {
    wmvList[i] = this->rbsList[i].mvs.wmv;
  }

  // 2. 撤消所有着法
  for (i = 1; i < nMoveNumSave; i ++) {
    UndoMakeMove();
  }

  // 3. 把所有棋子从棋盘上拿走，位置记录到"ucsqList"数组；
  for (i = 16; i < 48; i ++) {
    sq = this->ucsqPieces[i];
    ucsqList[i - 16] = sq;
    if (sq != 0) {
      AddPiece(sq, i, DEL_PIECE);
    }
  }

  // 4. 把拿走的棋子按照镜像位置重新放到棋盘上；
  for (i = 16; i < 48; i ++) {
    sq = ucsqList[i - 16];
    if (sq != 0) {
      AddPiece(SQUARE_MIRROR(sq), i);
    }
  }

  // 6. 还原镜像着法
  SetIrrev();
  for (i = 1; i < nMoveNumSave; i ++) {
    MakeMove(MOVE_MIRROR(wmvList[i]));
  }
}

// 以上是一些局面处理过程

// 以下是一些着法检测过程

// 着法合理性检测，仅用在“杀手着法”的检测中
bool PositionStruct::LegalMove(int mv) const {
  int sqSrc, sqDst, sqPin, pcMoved, pcCaptured, x, y, nSideTag;
  // 着法合理性检测包括以下几个步骤：

  // 1. 检查要走的子是否存在
  nSideTag = SIDE_TAG(this->sdPlayer);
  sqSrc = SRC(mv);
  sqDst = DST(mv);
  pcMoved = this->ucpcSquares[sqSrc];
  if ((pcMoved & nSideTag) == 0) {
    return false;
  }
  __ASSERT_SQUARE(sqSrc);
  __ASSERT_SQUARE(sqDst);
  __ASSERT_PIECE(pcMoved);

  // 2. 检查吃到的子是否为对方棋子(如果有吃子并且没有升变的话)
  pcCaptured = this->ucpcSquares[sqDst];
  if (sqSrc != sqDst && (pcCaptured & nSideTag) != 0) {
    return false;
  }
  __ASSERT_BOUND(0, PIECE_INDEX(pcMoved), 15);
  switch (PIECE_INDEX(pcMoved)) {

  // 3. 如果是帅(将)或仕(士)，则先看是否在九宫内，再看是否是合理位移
  case KING_FROM:
    return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
  case ADVISOR_FROM:
  case ADVISOR_TO:
    if (sqSrc == sqDst) {
      // 考虑升变，在底线并且兵(卒)不全时，才可升变
      return CAN_PROMOTE(sqSrc) && CanPromote();
    } else {
      return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
    }

  // 4. 如果是相(象)，则先看是否过河，再看是否是合理位移，最后看有没有被塞象眼
  case BISHOP_FROM:
  case BISHOP_TO:
    if (sqSrc == sqDst) {
      // 考虑升变，在底线并且兵(卒)不全时，才可升变
      return CAN_PROMOTE(sqSrc) && CanPromote();
    } else {
      return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) && this->ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
    }

  // 5. 如果是马，则先看看是否是合理位移，再看有没有被蹩马腿
  case KNIGHT_FROM:
  case KNIGHT_TO:
    sqPin = KNIGHT_PIN(sqSrc, sqDst);
    return sqPin != sqSrc && this->ucpcSquares[sqPin] == 0;

  // 6. 如果是车，则先看是横向移动还是纵向移动，再读取位行或位列的着法预生成数组
  case ROOK_FROM:
  case ROOK_TO:
    x = FILE_X(sqSrc);
    y = RANK_Y(sqSrc);
    if (x == FILE_X(sqDst)) {
      if (pcCaptured == 0) {
        return (FileMaskPtr(x, y)->wNonCap & PreGen.wBitFileMask[sqDst]) != 0;
      } else {
        return (FileMaskPtr(x, y)->wRookCap & PreGen.wBitFileMask[sqDst]) != 0;
      }
    } else if (y == RANK_Y(sqDst)) {
      if (pcCaptured == 0) {
        return (RankMaskPtr(x, y)->wNonCap & PreGen.wBitRankMask[sqDst]) != 0;
      } else {
        return (RankMaskPtr(x, y)->wRookCap & PreGen.wBitRankMask[sqDst]) != 0;
      }
    } else {
      return false;
    }

  // 7. 如果是炮，判断起来和车一样
  case CANNON_FROM:
  case CANNON_TO:
    x = FILE_X(sqSrc);
    y = RANK_Y(sqSrc);
    if (x == FILE_X(sqDst)) {
      if (pcCaptured == 0) {
        return (FileMaskPtr(x, y)->wNonCap & PreGen.wBitFileMask[sqDst]) != 0;
      } else {
        return (FileMaskPtr(x, y)->wCannonCap & PreGen.wBitFileMask[sqDst]) != 0;
      }
    } else if (y == RANK_Y(sqDst)) {
      if (pcCaptured == 0) {
        return (RankMaskPtr(x, y)->wNonCap & PreGen.wBitRankMask[sqDst]) != 0;
      } else {
        return (RankMaskPtr(x, y)->wCannonCap & PreGen.wBitRankMask[sqDst]) != 0;
      }
    } else {
      return false;
    }

  // 8. 如果是兵(卒)，则按红方和黑方分情况讨论
  default:
    if (AWAY_HALF(sqDst, this->sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
      return true;
    } else {
      return sqDst == SQUARE_FORWARD(sqSrc, this->sdPlayer);
    }
  }
}

// 将军检测
int PositionStruct::CheckedBy(bool bLazy) const {
  int pcCheckedBy, i, sqSrc, sqDst, sqPin, pc, x, y, nOppSideTag;
  SlideMaskStruct *lpsmsRank, *lpsmsFile;

  pcCheckedBy = 0;
  nOppSideTag = OPP_SIDE_TAG(this->sdPlayer);
  // 将军判断包括以下几部分内容：

  // 1. 判断帅(将)是否在棋盘上
  sqSrc = this->ucsqPieces[SIDE_TAG(this->sdPlayer)];
  if (sqSrc == 0) {
    return 0;
  }
  __ASSERT_SQUARE(sqSrc);

  // 2. 获得帅(将)所在格子的位行和位列
  x = FILE_X(sqSrc);
  y = RANK_Y(sqSrc);
  lpsmsRank = RankMaskPtr(x, y);
  lpsmsFile = FileMaskPtr(x, y);

  // 3. 判断是否将帅对脸
  sqDst = this->ucsqPieces[nOppSideTag + KING_FROM];
  if (sqDst != 0) {
    __ASSERT_SQUARE(sqDst);
    if (x == FILE_X(sqDst) && (lpsmsFile->wRookCap & PreGen.wBitFileMask[sqDst]) != 0) {
      return CHECK_MULTI;
    }
  }

  // 4. 判断是否被马将军
  for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
    sqDst = this->ucsqPieces[nOppSideTag + i];
    if (sqDst != 0) {
      __ASSERT_SQUARE(sqDst);
      sqPin = KNIGHT_PIN(sqDst, sqSrc); // 注意，sqSrc和sqDst是反的！
      if (sqPin != sqDst && this->ucpcSquares[sqPin] == 0) {
        if (bLazy || pcCheckedBy > 0) {
          return CHECK_MULTI;
        }
        pcCheckedBy = nOppSideTag + i;
        __ASSERT_PIECE(pcCheckedBy);
      }
    }
  }

  // 5. 判断是否被车将军或将帅对脸
  for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
    sqDst = this->ucsqPieces[nOppSideTag + i];
    if (sqDst != 0) {
      __ASSERT_SQUARE(sqDst);
      if (x == FILE_X(sqDst)) {
        if ((lpsmsFile->wRookCap & PreGen.wBitFileMask[sqDst]) != 0) {
          if (bLazy || pcCheckedBy > 0) {
            return CHECK_MULTI;
          }
          pcCheckedBy = nOppSideTag + i;
          __ASSERT_PIECE(pcCheckedBy);
        }
      } else if (y == RANK_Y(sqDst)) {
        if ((lpsmsRank->wRookCap & PreGen.wBitRankMask[sqDst]) != 0) {
          if (bLazy || pcCheckedBy > 0) {
            return CHECK_MULTI;
          }
          pcCheckedBy = nOppSideTag + i;
          __ASSERT_PIECE(pcCheckedBy);
        }
      }
    }
  }

  // 6. 判断是否被炮将军
  for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
    sqDst = this->ucsqPieces[nOppSideTag + i];
    if (sqDst != 0) {
      __ASSERT_SQUARE(sqDst);
      if (x == FILE_X(sqDst)) {
        if ((lpsmsFile->wCannonCap & PreGen.wBitFileMask[sqDst]) != 0) {
          if (bLazy || pcCheckedBy > 0) {
            return CHECK_MULTI;
          }
          pcCheckedBy = nOppSideTag + i;
          __ASSERT_PIECE(pcCheckedBy);
        }
      } else if (y == RANK_Y(sqDst)) {
        if ((lpsmsRank->wCannonCap & PreGen.wBitRankMask[sqDst]) != 0) {
          if (bLazy || pcCheckedBy > 0) {
            return CHECK_MULTI;
          }
          pcCheckedBy = nOppSideTag + i;
          __ASSERT_PIECE(pcCheckedBy);
        }
      }
    }
  }

  // 7. 判断是否被兵(卒)将军
  for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2) {
    // 如果帅(将)在边线(ElephantEye允许)，那么断言不成立
    // __ASSERT_SQUARE(sqDst);
    pc = this->ucpcSquares[sqDst];
    if ((pc & nOppSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
      if (bLazy || pcCheckedBy > 0) {
        return CHECK_MULTI;
      }
      pcCheckedBy = nOppSideTag + i;
      __ASSERT_PIECE(pcCheckedBy);
    }
  }
  pc = this->ucpcSquares[SQUARE_FORWARD(sqSrc, this->sdPlayer)];
  if ((pc & nOppSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
    if (bLazy || pcCheckedBy > 0) {
      return CHECK_MULTI;
    }
    pcCheckedBy = nOppSideTag + i;
    __ASSERT_PIECE(pcCheckedBy);
  }
  return pcCheckedBy;
}

// 判断是否被将死
bool PositionStruct::IsMate(void) {
  int i, nGenNum;
  MoveStruct mvsGen[MAX_GEN_MOVES];
  nGenNum = GenCapMoves(mvsGen);
  for (i = 0; i < nGenNum; i ++) {
    if (MakeMove(mvsGen[i].wmv)) {
      UndoMakeMove();
      return false;
    }
  }
  // 着法生成分两部分做，这样可以节约时间
  nGenNum = GenNonCapMoves(mvsGen);
  for (i = 0; i < nGenNum; i ++) {
    if (MakeMove(mvsGen[i].wmv)) {
      UndoMakeMove();
      return false;
    }
  }
  return true;
}

// 设置将军状态位
inline void SetPerpCheck(uint32_t &dwPerpCheck, int nChkChs) {
  if (nChkChs == 0) {
    dwPerpCheck = 0;
  } else if (nChkChs > 0) {
    dwPerpCheck &= 0x10000;
  } else {
    dwPerpCheck &= (1 << -nChkChs);
  }
}

// 重复局面检测
int PositionStruct::RepStatus(int nRecur) const {
  // 参数"nRecur"指重复次数，在搜索中取1以提高搜索效率(默认值)，根结点处取3以适应规则
  int sd;
  uint32_t dwPerpCheck, dwOppPerpCheck;
  const RollbackStruct *lprbs;
  /* 重复局面检测包括以下几个步骤：
   *
   * 1. 首先判断检测重复的迷你置换表中是否可能有当前局面，如果没有可能，就用不着判断了
   *    置换表"ucRepHash"是ElephantEye的一个特色，局面每执行一个着法时，就会在置换表项中记录下当前的"nMoveNum"
   *    如果置换表项已经填有其他局面，就不必覆盖了，参阅"MakeMove()"函数
   *    因此撤消着法时，只要查找置换表项的值是否等于当前的"nMoveNum"，如果相等则清空该项
   *    如果不等于当前的"nMoveNum"，则说明之前还有局面占有这个置换表项，不必清空该项，参阅"position.h"中的"UndoMakeMove()"函数
   */
  if (this->ucRepHash[this->zobr.dwKey & REP_HASH_MASK] == 0) {
    return REP_NONE;
  }

  // 2. 初始化
  sd = OPP_SIDE(this->sdPlayer);
  dwPerpCheck = dwOppPerpCheck = 0x1ffff;
  lprbs = this->rbsList + this->nMoveNum - 1;

  // 3. 检查上一个着法，如果是空着或吃子着法，就不可能有重复了
  while (lprbs->mvs.wmv != 0 && lprbs->mvs.CptDrw <= 0) {
    __ASSERT(lprbs >= this->rbsList);

    // 4. 判断双方的长打级别，0表示无长打，0xffff表示长捉，0x10000表示长将
    if (sd == this->sdPlayer) {
      SetPerpCheck(dwPerpCheck, lprbs->mvs.ChkChs);

      // 5. 寻找重复局面，如果重复次数达到预定次数，则返回重复记号
      if (lprbs->zobr.dwLock0 == this->zobr.dwLock0 && lprbs->zobr.dwLock1 == this->zobr.dwLock1) {
        nRecur --;
        if (nRecur == 0) {
          dwPerpCheck = ((dwPerpCheck & 0xffff) == 0 ? dwPerpCheck : 0xffff);
          dwOppPerpCheck = ((dwOppPerpCheck & 0xffff) == 0 ? dwOppPerpCheck : 0xffff);
          return dwPerpCheck > dwOppPerpCheck ? REP_LOSS : dwPerpCheck < dwOppPerpCheck ? REP_WIN : REP_DRAW;
        }
      }

    } else {
      SetPerpCheck(dwOppPerpCheck, lprbs->mvs.ChkChs);
    }

    sd = OPP_SIDE(sd);
    lprbs --;
  }
  return REP_NONE;
}

// 以上是一些着法检测过程
