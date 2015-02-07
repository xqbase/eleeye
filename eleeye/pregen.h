/*
pregen.h/pregen.cpp - Source Code for ElephantEye, Part II

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
#include "../base/rc4prng.h"

#ifndef PREGEN_H
#define PREGEN_H

#define __ASSERT_PIECE(pc) __ASSERT((pc) >= 16 && (pc) <= 47)
#define __ASSERT_SQUARE(sq) __ASSERT(IN_BOARD(sq))
#define __ASSERT_BITRANK(w) __ASSERT((w) < (1 << 9))
#define __ASSERT_BITFILE(w) __ASSERT((w) < (1 << 10))

const int RANK_TOP = 3;
const int RANK_BOTTOM = 12;
const int FILE_LEFT = 3;
const int FILE_CENTER = 7;
const int FILE_RIGHT = 11;

extern const bool cbcInBoard[256];    // 棋盘区域表
extern const bool cbcInFort[256];     // 城池区域表
extern const bool cbcCanPromote[256]; // 升变区域表
extern const int8_t ccLegalSpanTab[512];   // 合理着法跨度表
extern const int8_t ccKnightPinTab[512];   // 马腿表

inline bool IN_BOARD(int sq) {
  return cbcInBoard[sq];
}

inline bool IN_FORT(int sq) {
  return cbcInFort[sq];
}

inline bool CAN_PROMOTE(int sq) {
  return cbcCanPromote[sq];
}

inline int8_t LEGAL_SPAN_TAB(int nDisp) {
  return ccLegalSpanTab[nDisp];
}

inline int8_t KNIGHT_PIN_TAB(int nDisp) {
  return ccKnightPinTab[nDisp];
}

inline int RANK_Y(int sq) {
  return sq >> 4;
}

inline int FILE_X(int sq) {
  return sq & 15;
}

inline int COORD_XY(int x, int y) {
  return x + (y << 4);
}

inline int SQUARE_FLIP(int sq) {
  return 254 - sq;
}

inline int FILE_FLIP(int x) {
  return 14 - x;
}

inline int RANK_FLIP(int y) {
  return 15 - y;
}

inline int OPP_SIDE(int sd) {
  return 1 - sd;
}

inline int SQUARE_FORWARD(int sq, int sd) {
  return sq - 16 + (sd << 5);
}

inline int SQUARE_BACKWARD(int sq, int sd) {
  return sq + 16 - (sd << 5);
}

inline bool KING_SPAN(int sqSrc, int sqDst) {
  return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 1;
}

inline bool ADVISOR_SPAN(int sqSrc, int sqDst) {
  return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 2;
}

inline bool BISHOP_SPAN(int sqSrc, int sqDst) {
  return LEGAL_SPAN_TAB(sqDst - sqSrc + 256) == 3;
}

inline int BISHOP_PIN(int sqSrc, int sqDst) {
  return (sqSrc + sqDst) >> 1;
}

inline int KNIGHT_PIN(int sqSrc, int sqDst) {
  return sqSrc + KNIGHT_PIN_TAB(sqDst - sqSrc + 256);
}

inline bool WHITE_HALF(int sq) {
  return (sq & 0x80) != 0;
}

inline bool BLACK_HALF(int sq) {
  return (sq & 0x80) == 0;
}

inline bool HOME_HALF(int sq, int sd) {
  return (sq & 0x80) != (sd << 7);
}

inline bool AWAY_HALF(int sq, int sd) {
  return (sq & 0x80) == (sd << 7);
}

inline bool SAME_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) == 0;
}

inline bool DIFF_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) != 0;
}

inline int RANK_DISP(int y) {
  return y << 4;
}

inline int FILE_DISP(int x) {
  return x;
}

// 借助“位行”和“位列”生成车炮着法的预置结构
struct SlideMoveStruct {
  uint8_t ucNonCap[2];    // 不吃子能走到的最大一格/最小一格
  uint8_t ucRookCap[2];   // 车吃子能走到的最大一格/最小一格
  uint8_t ucCannonCap[2]; // 炮吃子能走到的最大一格/最小一格
  uint8_t ucSuperCap[2];  // 超级炮(隔两子吃子)能走到的最大一格/最小一格
}; // smv

// 借助“位行”和“位列”判断车炮着法合理性的预置结构
struct SlideMaskStruct {
  uint16_t wNonCap, wRookCap, wCannonCap, wSuperCap;
}; // sms

struct ZobristStruct {
  uint32_t dwKey, dwLock0, dwLock1;
  void InitZero(void) {
    dwKey = dwLock0 = dwLock1 = 0;
  }
  void InitRC4(RC4Struct &rc4) {
    dwKey = rc4.NextLong();
    dwLock0 = rc4.NextLong();
    dwLock1 = rc4.NextLong();
  }
  void Xor(const ZobristStruct &zobr) {
    dwKey ^= zobr.dwKey;
    dwLock0 ^= zobr.dwLock0;
    dwLock1 ^= zobr.dwLock1;
  }
  void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
    dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
    dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
    dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
  }
}; // zobr

extern struct PreGenStruct {
  // Zobrist键值表，分Zobrist键值和Zobrist校验锁两部分
  ZobristStruct zobrPlayer;
  ZobristStruct zobrTable[14][256];

  uint16_t wBitRankMask[256]; // 每个格子的位行的屏蔽位
  uint16_t wBitFileMask[256]; // 每个格子的位列的屏蔽位

  /* 借助“位行”和“位列”生成车炮着法和判断车炮着法合理性的预置数组
   *
   * “位行”和“位列”技术是ElephantEye的核心技术，用来处理车和炮的着法生成、将军判断和局面分析。
   * 以初始局面红方右边的炮在该列的行动为例，首先必须知道该列的“位列”，即"1010000101b"，
   * ElephantEye有两种预置数组，即"...MoveTab"和"...MaskTab"，用法分别是：
   * 一、如果要知道该子向前吃子的目标格(起始格是2，目标格是9)，那么希望查表就能知道这个格子，
   * 　　预先生成一个数组"FileMoveTab_CannonCap[10][1024]"，使得"FileMoveTab_CannonCap[2][1010000101b] == 9"就可以了。
   * 二、如果要判断该子能否吃到目标格(同样以起始格是2，目标格是9为例)，那么需要知道目标格的位列，即"0000000001b"，
   * 　　只要把"...MoveTab"的格子以“屏蔽位”的形式重新记作数组"...MaskTab"就可以了，用“与”操作来判断能否吃到目标格，
   * 　　通常一个"...MaskTab"单元会包括多个屏蔽位，判断能否吃到同行或同列的某个格子时，只需要做一次判断就可以了。
   */
  SlideMoveStruct smvRankMoveTab[9][512];   // 36,864 字节
  SlideMoveStruct smvFileMoveTab[10][1024]; // 81,920 字节
  SlideMaskStruct smsRankMaskTab[9][512];   // 36,864 字节
  SlideMaskStruct smsFileMaskTab[10][1024]; // 81,920 字节
                                            // 共计:  237,568 字节

  /* 其余棋子(不适合用“位行”和“位列”)的着法预生成数组
   *
   * 这部分数组是真正意义上的“着法预生成”数组，可以根据某个棋子的起始格直接查数组，得到所有的目标格。
   * 使用数组时，可以根据起始格来确定一个指针"g_...Moves[Square]"，这个指针指向一系列目标格，以0结束。
   * 为了对齐地址，数组[256][n]中n总是4的倍数，而且必须大于n(因为数组包括了结束标识符0)，除了象眼和马腿数组。
   */
  uint8_t ucsqKingMoves[256][8];
  uint8_t ucsqAdvisorMoves[256][8];
  uint8_t ucsqBishopMoves[256][8];
  uint8_t ucsqBishopPins[256][4];
  uint8_t ucsqKnightMoves[256][12];
  uint8_t ucsqKnightPins[256][8];
  uint8_t ucsqPawnMoves[2][256][4];
} PreGen;

// 局面预评价结构
extern struct PreEvalStruct {
  bool bPromotion;
  int vlAdvanced;
  uint8_t ucvlWhitePieces[7][256];
  uint8_t ucvlBlackPieces[7][256];
} PreEval;

void PreGenInit(void);

#endif
